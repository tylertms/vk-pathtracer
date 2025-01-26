#include "SceneLoader.h"
#include "GLTFLoader.h"

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

void loadSceneFromYAML(const std::string &filename, VKPT::SceneObject &sceneObj, std::vector<VKPT::Triangle> &triangleBuffer) {
    YAML::Node config = YAML::LoadFile(filename);

    if (!config["Objects"] || !config["Objects"].IsSequence()) return;

    for (int i = 0; i < config["Objects"].size(); i++) {
        YAML::Node object = config["Objects"][i];

        if (object["Mesh"]) {
            VKPT::Mesh mesh = object["Mesh"].as<VKPT::Mesh>();
            if (sceneObj.numMeshes < MAX_MESHES) {
                Loader::GLTFLoader loader(extractDirectory(filename) + object["Mesh"]["File"].as<std::string>());
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

}
