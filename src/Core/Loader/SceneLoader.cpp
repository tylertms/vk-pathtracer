#include "SceneLoader.h"
#include "GLTFLoader.h"

#include <fstream>
#include <iostream>
#include <vector>
#include <yaml-cpp/yaml.h>

namespace Loader {

std::string extractDirectory(const std::string& filepath) {
    size_t pos = filepath.find_last_of("/\\");

    if (pos != std::string::npos) {
        return filepath.substr(0, pos + 1);
    }

    return "";
}

void loadSceneFromYAML(const std::string &filename, std::vector<std::string> &modelPaths, VKPT::SceneObject &sceneObj, std::vector<VKPT::Triangle> &triangleBuffer) {
    YAML::Node config = YAML::LoadFile(filename);

    if (!config["Objects"] || !config["Objects"].IsSequence()) return;

    for (int i = 0; i < config["Objects"].size(); i++) {
        YAML::Node object = config["Objects"][i];

        if (object["Mesh"]) {
            VKPT::Mesh mesh = object["Mesh"].as<VKPT::Mesh>();
            if (sceneObj.numMeshes < MAX_MESHES) {
                Loader::GLTFLoader loader(extractDirectory(filename) + object["Mesh"]["File"].as<std::string>());
                modelPaths.push_back(object["Mesh"]["File"].as<std::string>());
                std::vector<VKPT::Triangle> triangles = loader.getTriangles();

                mesh.startIndex = triangleBuffer.size();
                mesh.triangleCount = triangles.size();

                triangleBuffer.reserve(mesh.startIndex + mesh.triangleCount);
                for (int j = 0; j < mesh.triangleCount; j++) {
                    sceneObj.triangles[mesh.startIndex + j] = triangles[j];
                    triangleBuffer.push_back(triangles[j]);
                }
                sceneObj.meshes[sceneObj.numMeshes++] = mesh;
                sceneObj.numTriangles += mesh.triangleCount;

            } else {
                std::cerr << "Warning: Maximum number of meshes reached.\n";
            }
        } else if (object["Sphere"]) {
            VKPT::Sphere sphere = object["Sphere"].as<VKPT::Sphere>();
            if (sceneObj.numSpheres < MAX_SPHERES) {
                sceneObj.spheres[sceneObj.numSpheres++] = sphere;
            } else {
                std::cerr << "Warning: Maximum number of spheres reached.\n";
            }
        }
    }
}

void saveSceneToYAML(const std::string &filename, VKPT::SceneObject &sceneObj, const std::vector<std::string> &modelPaths) {
    YAML::Node config;

    if (modelPaths.size() != sceneObj.numMeshes) {
        throw std::runtime_error("Number of model paths does not match the number of meshes.");
    }

    for (uint32_t i = 0; i < sceneObj.numMeshes; ++i) {
        YAML::Node meshProperties = YAML::Node(sceneObj.meshes[i]);
        meshProperties["File"] = modelPaths[i];

        YAML::Node meshNode;
        meshNode["Mesh"] = meshProperties;
        config["Objects"].push_back(meshNode);
    }

    for (uint32_t i = 0; i < sceneObj.numSpheres; ++i) {
        YAML::Node sphereProperties = YAML::Node(sceneObj.spheres[i]);

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
