#include "GLTFLoader.h"
#include "SceneLoader.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <cstring>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

#include "../BVH/Builder.h"

namespace File {

namespace {

constexpr uint32_t INVALID_MATERIAL_INDEX = std::numeric_limits<uint32_t>::max();

struct PrimitiveInstance {
    int meshIndex = -1;
    int primitiveIndex = -1;
    glm::mat3 transform = glm::mat3(0.0f);
    std::string sourcePath;
};

void logProgress(const std::string &msg) {
    std::cout << "[GLTF Loader] " << msg << std::endl;
}

template<typename T>
const T* getAccessorData(const tinygltf::Model &model, const tinygltf::Accessor &accessor) {
    const tinygltf::BufferView &bufferView = model.bufferViews[accessor.bufferView];
    const tinygltf::Buffer &buffer = model.buffers[bufferView.buffer];
    return reinterpret_cast<const T*>(buffer.data.data() + bufferView.byteOffset + accessor.byteOffset);
}

uint32_t appendMaterial(const VKPT::Material &material, Vulkan::SceneManager &sceneManager) {
    if (sceneManager.sceneData.numMaterials >= MAX_MATERIALS) {
        throw std::runtime_error("ERROR: Increase MAX_MATERIALS in Constants.h");
    }

    const uint32_t materialIndex = sceneManager.sceneData.numMaterials++;
    sceneManager.sceneData.materials[materialIndex] = material;
    return materialIndex;
}

void processMaterials(const tinygltf::Model &model, Vulkan::SceneManager &sceneManager, uint32_t textureOffset) {
    logProgress("Processing Materials...");
    for (const auto &gltfMaterial : model.materials) {
        VKPT::Material material;

        if (gltfMaterial.pbrMetallicRoughness.baseColorFactor.size() == 4) {
            material.baseColor = glm::vec3(
                static_cast<float>(gltfMaterial.pbrMetallicRoughness.baseColorFactor[0]),
                static_cast<float>(gltfMaterial.pbrMetallicRoughness.baseColorFactor[1]),
                static_cast<float>(gltfMaterial.pbrMetallicRoughness.baseColorFactor[2])
            );
        }
        if (gltfMaterial.pbrMetallicRoughness.baseColorTexture.index > -1) {
            material.baseColorTextureIndex = textureOffset + gltfMaterial.pbrMetallicRoughness.baseColorTexture.index;
        }
        if (gltfMaterial.pbrMetallicRoughness.metallicRoughnessTexture.index > -1) {
            material.metallicRoughnessTextureIndex = textureOffset + gltfMaterial.pbrMetallicRoughness.metallicRoughnessTexture.index;
        }
        if (gltfMaterial.normalTexture.index > -1) {
            material.normalTextureIndex = textureOffset + gltfMaterial.normalTexture.index;
        }
        if (gltfMaterial.occlusionTexture.index > -1) {
            logProgress("WARNING: Model has occlusion texture - currently unsupported.");
        }
        if (gltfMaterial.emissiveTexture.index > -1) {
            logProgress("WARNING: Model has emissive texture - currently unsupported.");
        }

        material.doubleSided = gltfMaterial.doubleSided;
        material.metallic = static_cast<float>(gltfMaterial.pbrMetallicRoughness.metallicFactor);
        material.roughness = static_cast<float>(gltfMaterial.pbrMetallicRoughness.roughnessFactor);
        appendMaterial(material, sceneManager);
    }
}

void processTextures(const std::string &basePath, const tinygltf::Model &model, Vulkan::SceneManager &sceneManager) {
    logProgress("Processing Textures...");
    for (const auto &texture : model.textures) {
        const int32_t imageIndex = texture.source;
        if (imageIndex < 0 || imageIndex >= static_cast<int>(model.images.size())) {
            continue;
        }

        if (sceneManager.sceneData.numTextures >= MAX_TEXTURES) {
            throw std::runtime_error("ERROR: Increase MAX_TEXTURES in Constants.h");
        }

        const tinygltf::Image &image = model.images[imageIndex];
        const std::string imageName = image.uri.empty() ? image.name : image.uri;
        if (imageName.empty()) {
            logProgress("WARNING: Embedded texture images are currently unsupported, skipping unnamed texture.");
            continue;
        }

        const std::string uri = basePath + imageName;
        const uint32_t textureIndex = sceneManager.sceneData.numTextures;
        sceneManager.loadTexture(uri, textureIndex);
        sceneManager.sceneData.numTextures++;
    }
}

glm::mat3 composeTransform(const glm::mat3 &parent, const glm::vec3 &translation, const glm::vec3 &rotation, const glm::vec3 &scale) {
    glm::mat3 transform = parent;
    transform[0] += translation;
    transform[1] += rotation;
    transform[2] *= scale;
    return transform;
}

glm::mat3 getNodeTransform(const glm::mat3 &parent, const tinygltf::Node &node) {
    glm::vec3 translation(0.0f);
    glm::vec3 rotation(0.0f);
    glm::vec3 scale(1.0f);

    if (node.matrix.size() == 16) {
        glm::mat4 matrix(1.0f);
        for (int col = 0; col < 4; ++col) {
            for (int row = 0; row < 4; ++row) {
                matrix[col][row] = static_cast<float>(node.matrix[col * 4 + row]);
            }
        }

        glm::vec3 skew;
        glm::vec4 perspective;
        glm::quat orientation;
        if (glm::decompose(matrix, scale, orientation, translation, skew, perspective)) {
            rotation = glm::degrees(glm::eulerAngles(glm::normalize(orientation)));
        }
    } else {
        if (node.translation.size() == 3) {
            translation = glm::vec3(
                static_cast<float>(node.translation[0]),
                static_cast<float>(node.translation[1]),
                static_cast<float>(node.translation[2])
            );
        }
        if (node.rotation.size() == 4) {
            rotation = glm::degrees(glm::eulerAngles(glm::qua(
                static_cast<float>(node.rotation[3]),
                static_cast<float>(node.rotation[0]),
                static_cast<float>(node.rotation[1]),
                static_cast<float>(node.rotation[2])
            )));
        }
        if (node.scale.size() == 3) {
            scale = glm::vec3(
                static_cast<float>(node.scale[0]),
                static_cast<float>(node.scale[1]),
                static_cast<float>(node.scale[2])
            );
        }
    }

    return composeTransform(parent, translation, rotation, scale);
}

void collectNodeInstances(
    int nodeIndex,
    const glm::mat3 &parentTransform,
    const tinygltf::Model &model,
    const std::string &sourcePath,
    Vulkan::SceneManager &sceneManager,
    std::vector<PrimitiveInstance> &instances)
{
    const tinygltf::Node &node = model.nodes[nodeIndex];
    const glm::mat3 nodeTransform = getNodeTransform(parentTransform, node);

    if (node.mesh >= 0) {
        const tinygltf::Mesh &mesh = model.meshes[node.mesh];
        logProgress("Processing Mesh: " + mesh.name);
        for (size_t primitiveIndex = 0; primitiveIndex < mesh.primitives.size(); ++primitiveIndex) {
            PrimitiveInstance instance;
            instance.meshIndex = node.mesh;
            instance.primitiveIndex = static_cast<int>(primitiveIndex);
            instance.transform = nodeTransform;
            instance.sourcePath = sourcePath;
            instances.push_back(instance);
        }
    }

    if (node.camera >= 0) {
        const tinygltf::Camera &camera = model.cameras[node.camera];
        glm::mat3 cameraTransform = nodeTransform;
        sceneManager.addCamera(node.name, camera, cameraTransform);
    }

    for (int childIndex : node.children) {
        collectNodeInstances(childIndex, nodeTransform, model, sourcePath, sceneManager, instances);
    }
}

std::vector<PrimitiveInstance> collectPrimitiveInstances(
    const tinygltf::Model &model,
    const std::string &sourcePath,
    const glm::mat3 &importTransform,
    Vulkan::SceneManager &sceneManager)
{
    logProgress("Processing Nodes...");

    std::vector<PrimitiveInstance> instances;
    for (const auto &scene : model.scenes) {
        logProgress("Processing Scene: " + scene.name);
        for (int nodeIndex : scene.nodes) {
            collectNodeInstances(nodeIndex, importTransform, model, sourcePath, sceneManager, instances);
        }
    }

    if (!instances.empty()) {
        return instances;
    }

    logProgress("No scene nodes referenced mesh primitives, falling back to raw mesh list.");
    for (size_t meshIndex = 0; meshIndex < model.meshes.size(); ++meshIndex) {
        const tinygltf::Mesh &mesh = model.meshes[meshIndex];
        for (size_t primitiveIndex = 0; primitiveIndex < mesh.primitives.size(); ++primitiveIndex) {
            PrimitiveInstance instance;
            instance.meshIndex = static_cast<int>(meshIndex);
            instance.primitiveIndex = static_cast<int>(primitiveIndex);
            instance.transform = importTransform;
            instance.sourcePath = sourcePath;
            instances.push_back(instance);
        }
    }

    return instances;
}

void loadTrianglesFromPrimitive(const tinygltf::Model &model, const tinygltf::Primitive &primitive, std::vector<VKPT::Triangle> &triangles) {
    const auto posIt = primitive.attributes.find("POSITION");
    const auto normIt = primitive.attributes.find("NORMAL");
    if (posIt == primitive.attributes.end() || normIt == primitive.attributes.end()) {
        std::cerr << "Primitive missing POSITION or NORMAL, skipping." << std::endl;
        return;
    }

    const tinygltf::Accessor &posAccessor = model.accessors[posIt->second];
    const float *positions = getAccessorData<float>(model, posAccessor);

    const tinygltf::Accessor &normAccessor = model.accessors[normIt->second];
    const float *normals = getAccessorData<float>(model, normAccessor);

    const float *tangents = nullptr;
    const auto tangentIt = primitive.attributes.find("TANGENT");
    if (tangentIt != primitive.attributes.end()) {
        tangents = getAccessorData<float>(model, model.accessors[tangentIt->second]);
    }

    const float *uvs0 = nullptr;
    const auto uvIt = primitive.attributes.find("TEXCOORD_0");
    if (uvIt != primitive.attributes.end()) {
        uvs0 = getAccessorData<float>(model, model.accessors[uvIt->second]);
    }

    std::vector<unsigned int> indices;
    if (primitive.indices >= 0) {
        const tinygltf::Accessor &indexAccessor = model.accessors[primitive.indices];
        indices.resize(indexAccessor.count);
        const void *indexData = getAccessorData<unsigned char>(model, indexAccessor);
        if (indexAccessor.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT) {
            const auto *buffer = reinterpret_cast<const unsigned short*>(indexData);
            for (size_t i = 0; i < indexAccessor.count; ++i) {
                indices[i] = static_cast<unsigned int>(buffer[i]);
            }
        } else if (indexAccessor.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT) {
            const auto *buffer = reinterpret_cast<const unsigned int*>(indexData);
            std::memcpy(indices.data(), buffer, indexAccessor.count * sizeof(unsigned int));
        } else {
            std::cerr << "Unsupported index type: " << indexAccessor.componentType << std::endl;
            return;
        }
    } else {
        indices.resize(posAccessor.count);
        for (size_t i = 0; i < posAccessor.count; ++i) {
            indices[i] = static_cast<unsigned int>(i);
        }
    }

    const size_t triangleCount = indices.size() / 3;
    triangles.reserve(triangles.size() + triangleCount);
    for (size_t triangleIndex = 0; triangleIndex < triangleCount; ++triangleIndex) {
        VKPT::Triangle triangle{};
        for (int vertex = 0; vertex < 3; ++vertex) {
            const unsigned int index = indices[triangleIndex * 3 + vertex];
            std::memcpy(glm::value_ptr(triangle.pos[vertex]), positions + index * 3, sizeof(float) * 3);
            std::memcpy(glm::value_ptr(triangle.norm[vertex]), normals + index * 3, sizeof(float) * 3);
            if (tangents) {
                std::memcpy(glm::value_ptr(triangle.tan[vertex]), tangents + index * 4, sizeof(float) * 4);
            }
            if (uvs0) {
                std::memcpy(glm::value_ptr(triangle.uv0[vertex]), uvs0 + index * 2, sizeof(float) * 2);
            }
        }
        triangles.push_back(triangle);
    }
}

uint32_t resolveMaterialIndex(
    const tinygltf::Primitive &primitive,
    const uint32_t materialOffset,
    const uint32_t materialOverride,
    uint32_t &fallbackMaterialIndex,
    Vulkan::SceneManager &sceneManager)
{
    if (materialOverride != INVALID_MATERIAL_INDEX) {
        return materialOverride;
    }

    if (primitive.material >= 0) {
        return materialOffset + static_cast<uint32_t>(primitive.material);
    }

    if (fallbackMaterialIndex == INVALID_MATERIAL_INDEX) {
        fallbackMaterialIndex = appendMaterial(VKPT::Material{}, sceneManager);
    }
    return fallbackMaterialIndex;
}

} // namespace

void loadGLTF(const std::string &filename, const glm::mat3 &transform, uint32_t matIndex, Vulkan::SceneManager &sceneManager) {
    if (filename.empty()) {
        return;
    }

    logProgress("Loading model: " + filename);
    tinygltf::TinyGLTF loader;
    const bool isBinary = filename.find(".glb") != std::string::npos;
    tinygltf::Model model;
    std::string error;
    std::string warning;
    const bool ok = isBinary
        ? loader.LoadBinaryFromFile(&model, &error, &warning, filename)
        : loader.LoadASCIIFromFile(&model, &error, &warning, filename);

    if (!warning.empty()) {
        std::cout << "Warning: " << warning << std::endl;
    }
    if (!error.empty()) {
        std::cerr << "Error: " << error << std::endl;
    }
    if (!ok) {
        std::cerr << "Failed to parse glTF file: " << filename << std::endl;
        return;
    }
    logProgress("Model loaded successfully!");

    const bool importSourceMaterials = matIndex == INVALID_MATERIAL_INDEX;
    const uint32_t textureOffset = sceneManager.sceneData.numTextures;
    const uint32_t materialOffset = sceneManager.sceneData.numMaterials;

    if (importSourceMaterials) {
        processMaterials(model, sceneManager, textureOffset);
        processTextures(extractDirectory(filename), model, sceneManager);
    }

    const std::vector<PrimitiveInstance> instances = collectPrimitiveInstances(model, filename, transform, sceneManager);

    uint32_t fallbackMaterialIndex = INVALID_MATERIAL_INDEX;
    for (const PrimitiveInstance &instance : instances) {
        const tinygltf::Mesh &sourceMesh = model.meshes[instance.meshIndex];
        const tinygltf::Primitive &primitive = model.meshes[instance.meshIndex].primitives[instance.primitiveIndex];

        std::vector<VKPT::Triangle> primitiveTriangles;
        loadTrianglesFromPrimitive(model, primitive, primitiveTriangles);
        if (primitiveTriangles.empty()) {
            continue;
        }

        if (sceneManager.sceneData.numMeshes >= MAX_MESHES) {
            throw std::runtime_error("ERROR: Increase MAX_MESHES in Constants.h");
        }
        if (sceneManager.sceneData.numTriangles + primitiveTriangles.size() >= MAX_TRIANGLES) {
            printf("Triangle count %d exceeds capacity %d!\n",
                static_cast<int>(sceneManager.sceneData.numTriangles + primitiveTriangles.size()), MAX_TRIANGLES);
            throw std::runtime_error("ERROR: Increase MAX_TRIANGLES in Constants.h");
        }

        const uint32_t triStartIndex = sceneManager.sceneData.numTriangles;
        for (const VKPT::Triangle &triangle : primitiveTriangles) {
            sceneManager.triIndices.push_back(sceneManager.sceneData.numTriangles);

            const glm::vec3 p0(triangle.pos[0]);
            const glm::vec3 p1(triangle.pos[1]);
            const glm::vec3 p2(triangle.pos[2]);
            sceneManager.triMin.push_back(glm::min(p0, glm::min(p1, p2)));
            sceneManager.triMax.push_back(glm::max(p0, glm::max(p1, p2)));
            sceneManager.triCentroid.push_back((p0 + p1 + p2) / 3.0f);
            sceneManager.sceneStorage->triangles[sceneManager.sceneData.numTriangles] = triangle;
            sceneManager.sceneData.numTriangles++;
        }

        VKPT::Mesh mesh{};
        mesh.materialIndex = resolveMaterialIndex(primitive, materialOffset, matIndex, fallbackMaterialIndex, sceneManager);
        mesh.rootBVHNode = sceneManager.sceneData.numBVHs;

        const uint32_t meshIndex = sceneManager.sceneData.numMeshes++;
        sceneManager.sceneData.meshes[meshIndex] = mesh;
        sceneManager.modelPaths.push_back(filename);
        std::string meshName = sourceMesh.name.empty() ? extractFilename(filename) : sourceMesh.name;
        if (sourceMesh.primitives.size() > 1) {
            meshName += " - " + std::to_string(instance.primitiveIndex);
        }
        sceneManager.meshNames.push_back(meshName);
        sceneManager.meshTransforms.push_back(instance.transform);

        VKPT::BVH root;
        root.index = triStartIndex;
        root.triangleCount = static_cast<uint32_t>(primitiveTriangles.size());
        BVH::createBVH(sceneManager.sceneData.meshes[meshIndex], root, sceneManager);

        for (uint32_t triangleIndex = triStartIndex; triangleIndex < triStartIndex + primitiveTriangles.size(); ++triangleIndex) {
            const uint32_t sourceIndex = sceneManager.triIndices[triangleIndex] - triStartIndex;
            sceneManager.sceneStorage->triangles[triangleIndex] = primitiveTriangles[sourceIndex];
        }
    }

    sceneManager.updateMeshTransforms();
    sceneManager.uploadFullSceneStorage();
    sceneManager.resetAccumulation();
    logProgress("Finished loading model!");
}

} // namespace File
