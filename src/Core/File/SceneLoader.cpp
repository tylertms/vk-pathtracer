#include "SceneLoader.h"

#include "GLTFLoader.h"

#include <filesystem>
#include <fstream>
#include <vector>
#include <yaml-cpp/yaml.h>

namespace fs = std::filesystem;

namespace File {

std::string extractDirectory(const std::string &filepath) {
    size_t pos = filepath.find_last_of("/\\");
    return (pos != std::string::npos) ? filepath.substr(0, pos + 1) : "";
}

std::string extractFilename(const std::string& filepath) {
    int lastSlashPos = max((int)filepath.find_last_of('/'), (int)filepath.find_last_of('\\'));
    int lastDotPos = filepath.find_last_of('.');

    if (lastSlashPos < 0 || lastDotPos < 0) return "";

    if (lastSlashPos == std::string::npos) {
        lastSlashPos = -1; 
    }

    if (lastDotPos != std::string::npos && lastDotPos > lastSlashPos) {
        return filepath.substr(lastSlashPos + 1, lastDotPos - lastSlashPos - 1);
    } else if (lastSlashPos != std::string::npos) {
        return filepath.substr(lastSlashPos + 1);
    }
    return "";
}

void loadSceneFromYAML(const std::string filename, Vulkan::SceneManager &sceneManager) {
    if (filename.empty())
        return;

    YAML::Node config = YAML::LoadFile(filename);
    if (!config["Objects"] || !config["Objects"].IsSequence())
        return;

    sceneManager.reset();

    if (config["Camera"] && config["Camera"].IsMap()) {
        VKPT::Camera camera = config["Camera"].as<VKPT::Camera>();
        sceneManager.sceneData.camera = camera;
    }

    if (config["EnvTexture"]) {
        std::string loadpath = config["EnvTexture"].as<std::string>();
        if (fs::path(loadpath).is_relative()) {
            loadpath = std::string(extractDirectory(filename) + loadpath);
        }
    }

    for (uint32_t i = 0; i < config["Objects"].size(); i++) {
        YAML::Node object = config["Objects"][i];

        if (object["Mesh"]) {
            glm::mat3 transform = object["Mesh"]["Transform"].as<glm::mat3>();
            uint32_t matIndex = object["Mesh"]["MaterialIndex"].as<uint32_t>();
            std::string loadpath = object["Mesh"]["File"].as<std::string>();

            if (fs::path(loadpath).is_relative()) {
                loadpath = std::string(extractDirectory(filename) + loadpath);
            }

            loadGLTF(loadpath, transform, matIndex, sceneManager);            
        }

        else if (object["Sphere"]) {
            VKPT::Sphere sphere = object["Sphere"].as<VKPT::Sphere>();
            sceneManager.sceneData.spheres[sceneManager.sceneData.numSpheres++] = sphere;
        }
    }

    for (uint32_t i = 0; i < config["Materials"].size(); i++) {
        sceneManager.sceneData.materials[sceneManager.sceneData.numMaterials++] = 
            config["Materials"][i]["Material" + std::to_string(i)].as<VKPT::Material>();
    }
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
        config["EnvTexture"] = sceneManager.texturePaths[0];
    }

    for (uint32_t i = 0; i < sceneManager.sceneData.numMeshes; i++) {
        const VKPT::Mesh &mesh = sceneManager.sceneData.meshes[i];
        YAML::Node meshProperties = YAML::Node(mesh);
        meshProperties["File"] = sceneManager.modelPaths[i];
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
