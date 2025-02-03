#include "SceneLoader.h"

#include <filesystem>
#include <fstream>
#include <vector>
#include <yaml-cpp/yaml.h>

namespace fs = std::filesystem;

namespace IO {

std::string extractDirectory(const std::string& filepath) {
    size_t pos = filepath.find_last_of("/\\");

    if (pos != std::string::npos) {
        return filepath.substr(0, pos + 1);
    }

    return "";
}

void loadSceneFromYAML(const std::string filename, Vulkan::SceneManager &sceneManager) {
    if (filename.empty()) return;

    YAML::Node config = YAML::LoadFile(filename);
    if (!config["Objects"] || !config["Objects"].IsSequence()) return;

    sceneManager.reset();

    if (config["Camera"] && config["Camera"].IsMap()) {
        VKPT::Camera camera = config["Camera"].as<VKPT::Camera>();
        sceneManager.sceneData.camera = camera;
    }

    for (uint32_t i = 0; i < config["Objects"].size(); i++) {
        YAML::Node object = config["Objects"][i];

        if (object["Mesh"]) {
            glm::mat3 transform = object["Mesh"]["Transform"].as<glm::mat3>();
            std::string loadpath = object["Mesh"]["File"].as<std::string>();

            if (fs::path(loadpath).is_relative()) {
                loadpath = std::string(extractDirectory(filename) + object["Mesh"]["File"].as<std::string>());
            }

            VKPT::Mesh mesh = object["Mesh"].as<VKPT::Mesh>();
            uint32_t startMeshCount = sceneManager.sceneData.numMeshes;

            sceneManager.addMesh(loadpath);

            uint32_t meshCount = sceneManager.sceneData.numMeshes - startMeshCount;
            for (uint32_t i = 0; i < meshCount; i++) {
                uint32_t index = startMeshCount + i;
                sceneManager.sceneData.meshes[index].material = mesh.material;
                sceneManager.meshTransforms[index] = transform;
            }
        }

        else if (object["Sphere"]) {
            VKPT::Sphere sphere = object["Sphere"].as<VKPT::Sphere>();
            sceneManager.sceneData.spheres[sceneManager.sceneData.numSpheres++] = sphere;
        }
    }

    sceneManager.updateMeshTransforms();
    sceneManager.uploadFullSceneStorage();
}

void saveSceneToYAML(const std::string filename, const Vulkan::SceneManager &sceneManager) {
    if (filename.empty()) return;

    YAML::Node config;

    if (sceneManager.modelPaths.size() != sceneManager.sceneData.numMeshes) {
        throw std::runtime_error("Number of model paths does not match the number of meshes.");
    }

    config["Camera"] = sceneManager.sceneData.camera;

    for (uint32_t i = 0; i < sceneManager.sceneData.numMeshes; i++) {
        YAML::Node meshProperties = YAML::Node(sceneManager.sceneData.meshes[i]);
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

    std::ofstream fout(filename);
    if (!fout.is_open()) {
        throw std::runtime_error("Failed to open file for writing: " + filename);
    }
    fout << config;
    fout.close();
}

}
