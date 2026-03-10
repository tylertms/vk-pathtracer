#include "SceneLoader.h"

#include "GLTFLoader.h"

#include <filesystem>
#include <fstream>
#include <limits>
#include <stdexcept>
#include <vector>
#include <yaml-cpp/yaml.h>

namespace fs = std::filesystem;

namespace File {

namespace {

constexpr uint32_t INVALID_MATERIAL_INDEX = std::numeric_limits<uint32_t>::max();

glm::mat3 makeIdentityTransform() {
    glm::mat3 transform(0.0f);
    transform[2] = glm::vec3(1.0f);
    return transform;
}

std::string resolvePath(const std::string &sceneFilename, const std::string &assetPath) {
    if (assetPath.empty()) {
        return assetPath;
    }

    fs::path path(assetPath);
    if (path.is_absolute()) {
        return path.lexically_normal().string();
    }

    return (fs::path(extractDirectory(sceneFilename)) / path).lexically_normal().string();
}

std::string makeSceneRelativePath(const std::string &sceneFilename, const std::string &assetPath) {
    if (assetPath.empty()) {
        return assetPath;
    }

    try {
        const fs::path sceneDirectory = fs::absolute(fs::path(sceneFilename)).parent_path();
        const fs::path assetAbsolute = fs::absolute(fs::path(assetPath));
        return fs::relative(assetAbsolute, sceneDirectory).generic_string();
    } catch (...) {
        return assetPath;
    }
}

uint32_t appendMaterial(const YAML::Node &materialNode, Vulkan::SceneManager &sceneManager) {
    if (!materialNode || !materialNode.IsMap()) {
        return INVALID_MATERIAL_INDEX;
    }
    if (sceneManager.sceneData.numMaterials >= MAX_MATERIALS) {
        throw std::runtime_error("ERROR: Increase MAX_MATERIALS in Constants.h");
    }

    const uint32_t materialIndex = sceneManager.sceneData.numMaterials++;
    sceneManager.sceneData.materials[materialIndex] = materialNode.as<VKPT::Material>();
    return materialIndex;
}

uint32_t ensureDefaultMaterial(Vulkan::SceneManager &sceneManager) {
    if (sceneManager.sceneData.numMaterials == 0) {
        if (sceneManager.sceneData.numMaterials >= MAX_MATERIALS) {
            throw std::runtime_error("ERROR: Increase MAX_MATERIALS in Constants.h");
        }
        sceneManager.sceneData.materials[sceneManager.sceneData.numMaterials++] = VKPT::Material{};
    }
    return 0;
}

uint32_t resolveMaterialIndex(const YAML::Node &objectNode, Vulkan::SceneManager &sceneManager, uint32_t fallback = INVALID_MATERIAL_INDEX) {
    if (objectNode["MaterialIndex"]) {
        return objectNode["MaterialIndex"].as<uint32_t>();
    }
    if (objectNode["Material"]) {
        return appendMaterial(objectNode["Material"], sceneManager);
    }
    if (fallback != INVALID_MATERIAL_INDEX) {
        return fallback;
    }
    return ensureDefaultMaterial(sceneManager);
}

} // namespace

std::string extractDirectory(const std::string &filepath) {
    size_t pos = filepath.find_last_of("/\\");
    return (pos != std::string::npos) ? filepath.substr(0, pos + 1) : "";
}

std::string extractFilename(const std::string& filepath) {
    return fs::path(filepath).stem().string();
}

void loadSceneFromYAML(const std::string filename, Vulkan::SceneManager &sceneManager) {
    if (filename.empty())
        return;

    YAML::Node config = YAML::LoadFile(filename);

    sceneManager.reset();

    if (config["Camera"] && config["Camera"].IsMap()) {
        VKPT::Camera camera = config["Camera"].as<VKPT::Camera>();
        sceneManager.sceneData.camera = camera;
    }

    if (config["EnvTexture"]) {
        sceneManager.updateEnvTexture(resolvePath(filename, config["EnvTexture"].as<std::string>()));
    }

    if (config["Materials"] && config["Materials"].IsSequence()) {
        for (uint32_t i = 0; i < config["Materials"].size(); i++) {
            const YAML::Node materialNode = config["Materials"][i];
            if (!materialNode || !materialNode.IsMap() || materialNode.size() == 0) {
                continue;
            }

            appendMaterial(materialNode.begin()->second, sceneManager);
        }
    }

    if (!config["Objects"] || !config["Objects"].IsSequence()) {
        sceneManager.uploadFullSceneStorage();
        sceneManager.resetAccumulation();
        return;
    }

    for (uint32_t i = 0; i < config["Objects"].size(); i++) {
        const YAML::Node object = config["Objects"][i];

        if (object["Mesh"]) {
            const YAML::Node meshNode = object["Mesh"];
            if (!meshNode["File"]) {
                continue;
            }

            const glm::mat3 transform = meshNode["Transform"] ? meshNode["Transform"].as<glm::mat3>() : makeIdentityTransform();
            const uint32_t materialIndex = resolveMaterialIndex(meshNode, sceneManager, INVALID_MATERIAL_INDEX);
            const std::string loadpath = resolvePath(filename, meshNode["File"].as<std::string>());
            loadGLTF(loadpath, transform, materialIndex, sceneManager);
        } else if (object["Sphere"]) {
            YAML::Node sphereNode = object["Sphere"];
            VKPT::Sphere sphere = sphereNode.as<VKPT::Sphere>();
            sphere.materialIndex = resolveMaterialIndex(sphereNode, sceneManager, sphere.materialIndex);
            sceneManager.sceneData.spheres[sceneManager.sceneData.numSpheres++] = sphere;
        }
    }

    sceneManager.uploadFullSceneStorage();
    sceneManager.resetAccumulation();
}

void saveSceneToYAML(const std::string filename, const Vulkan::SceneManager &sceneManager) {
    if (filename.empty())
        return;

    YAML::Node config;

    if (sceneManager.modelPaths.size() != sceneManager.sceneData.numMeshes) {
        throw std::runtime_error("Number of model paths does not match the number of meshes.");
    }

    config["Camera"] = sceneManager.sceneData.camera;

    if (sceneManager.texturePaths.size() > 0 && !sceneManager.texturePaths[0].empty()) {
        config["EnvTexture"] = makeSceneRelativePath(filename, sceneManager.texturePaths[0]);
    }

    for (uint32_t i = 0; i < sceneManager.sceneData.numMeshes; i++) {
        const VKPT::Mesh &mesh = sceneManager.sceneData.meshes[i];
        YAML::Node meshProperties = YAML::Node(mesh);
        meshProperties["File"] = makeSceneRelativePath(filename, sceneManager.modelPaths[i]);
        meshProperties["Transform"] = sceneManager.meshTransforms[i];

        YAML::Node meshNode;
        meshNode["Mesh"] = meshProperties;
        config["Objects"].push_back(meshNode);
    }

    for (uint32_t i = 0; i < sceneManager.sceneData.numSpheres; i++) {
        YAML::Node sphereProperties = YAML::Node(sceneManager.sceneData.spheres[i]);

        YAML::Node sphereNode;
        sphereNode["Sphere"] = sphereProperties;
        config["Objects"].push_back(sphereNode);
    }

    for (uint32_t i = 0; i < sceneManager.sceneData.numMaterials; i++) {
        YAML::Node material = YAML::Node(sceneManager.sceneData.materials[i]);

        YAML::Node materialNode;
        materialNode["Material" + std::to_string(i)] = material;
        config["Materials"].push_back(materialNode);
    }

    std::ofstream fout(filename);
    if (!fout.is_open()) {
        throw std::runtime_error("Failed to open file for writing: " + filename);
    }
    fout << config;
    fout.close();
}

} // namespace File
