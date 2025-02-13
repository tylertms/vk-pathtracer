#include "GLTFLoader.h"
#include "FilePicker.h"
#include "SceneLoader.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

#include <iostream>
#include <vector>
#include <cstring>
#include <functional>
#include <stdexcept>

#include "../BVH/Builder.h"

namespace File {

// -----------------------------------------------------------------------------
// Logging helper: prints progress messages
// -----------------------------------------------------------------------------
static void logProgress(const std::string &msg) {
    std::cout << "[GLTF Loader] " << msg << std::endl;
}

// -----------------------------------------------------------------------------
// Helper: Get a pointer to accessor data for a given type T
// -----------------------------------------------------------------------------
template<typename T>
const T* getAccessorData(const tinygltf::Model &model, const tinygltf::Accessor &accessor) {
    const tinygltf::BufferView &bufferView = model.bufferViews[accessor.bufferView];
    const tinygltf::Buffer &buffer = model.buffers[bufferView.buffer];
    return reinterpret_cast<const T*>(buffer.data.data() + bufferView.byteOffset + accessor.byteOffset);
}

// -----------------------------------------------------------------------------
// Process Materials
// -----------------------------------------------------------------------------
static void processMaterials(const tinygltf::Model &model, Vulkan::SceneManager &sceneManager, uint32_t textureOffset) {
    logProgress("Processing Materials...");
    for (const auto &gltfMaterial : model.materials) {
        if (sceneManager.sceneData.numMaterials >= MAX_MATERIALS)
            throw std::runtime_error("ERROR: Increase MAX_MATERIALS in Constants.h");
        uint32_t matIndex = sceneManager.sceneData.numMaterials++;
        VKPT::Material &material = sceneManager.sceneData.materials[matIndex];

        if (gltfMaterial.pbrMetallicRoughness.baseColorFactor.size() == 4)
            material.baseColor = glm::vec3(
                static_cast<float>(gltfMaterial.pbrMetallicRoughness.baseColorFactor[0]),
                static_cast<float>(gltfMaterial.pbrMetallicRoughness.baseColorFactor[1]),
                static_cast<float>(gltfMaterial.pbrMetallicRoughness.baseColorFactor[2])
            );
        if (gltfMaterial.pbrMetallicRoughness.baseColorTexture.index > -1)
            material.baseColorTextureIndex = textureOffset + gltfMaterial.pbrMetallicRoughness.baseColorTexture.index;
        if (gltfMaterial.pbrMetallicRoughness.metallicRoughnessTexture.index > -1)
            material.metallicRoughnessTextureIndex = textureOffset + gltfMaterial.pbrMetallicRoughness.metallicRoughnessTexture.index;
        if (gltfMaterial.normalTexture.index > -1)
            material.normalTextureIndex = textureOffset + gltfMaterial.normalTexture.index;
        if (gltfMaterial.occlusionTexture.index > -1)
            logProgress("WARNING: Model has occlusion texture - currently unsupported.");
        if (gltfMaterial.emissiveTexture.index > -1)
            logProgress("WARNING: Model has emissive texture - currently unsupported.");

        material.doubleSided = gltfMaterial.doubleSided;
        material.roughness = static_cast<float>(gltfMaterial.pbrMetallicRoughness.roughnessFactor);
    }
}

// -----------------------------------------------------------------------------
// Process Textures
// -----------------------------------------------------------------------------
static void processTextures(const std::string &basePath, const tinygltf::Model &model, Vulkan::SceneManager &sceneManager) {
    logProgress("Processing Textures...");
    for (const auto &texture : model.textures) {
        int32_t imageIndex = texture.source;
        if (imageIndex < 0 || imageIndex >= static_cast<int>(model.images.size()))
            continue;
        const tinygltf::Image &image = model.images[imageIndex];
        std::string uri = basePath + image.uri;
        uint32_t textureIndex = sceneManager.sceneData.numTextures;
        sceneManager.loadTexture(uri, textureIndex);
        sceneManager.sceneData.numTextures++;
    }
}

// -----------------------------------------------------------------------------
// Get the node transformation using your original mat3 translation/euler rotation/scale scheme
// -----------------------------------------------------------------------------
static glm::mat3 getNodeTransform(const glm::mat3 &parent, const tinygltf::Node &node) {
    glm::mat3 transform = parent;

    glm::vec3 translation(0.0f);
    if (node.translation.size() == 3)
        translation = glm::vec3(
            static_cast<float>(node.translation[0]),
            static_cast<float>(node.translation[1]),
            static_cast<float>(node.translation[2])
        );
    glm::vec3 rotation(0.0f);
    if (node.rotation.size() == 4)
        rotation = glm::degrees(glm::eulerAngles(glm::qua(
            static_cast<float>(node.rotation[3]),
            static_cast<float>(node.rotation[0]),
            static_cast<float>(node.rotation[1]),
            static_cast<float>(node.rotation[2])
        )));
    glm::vec3 scale(1.0f);
    if (node.scale.size() == 3)
        scale = glm::vec3(
            static_cast<float>(node.scale[0]),
            static_cast<float>(node.scale[1]),
            static_cast<float>(node.scale[2])
        );

    // In this scheme, column 0 stores translation, column 1 stores rotation (Euler angles), and column 2 stores scale.
    transform[0] += translation;
    transform[1] += rotation;
    transform[2] *= scale;

    return transform;
}

// -----------------------------------------------------------------------------
// Recursively process a node and its children using mat3 transforms
// -----------------------------------------------------------------------------
static void processNode(int nodeIndex, const glm::mat3 &parentTransform, const tinygltf::Model &model, Vulkan::SceneManager &sceneManager) {
    const tinygltf::Node &node = model.nodes[nodeIndex];
    glm::mat3 nodeTransform = getNodeTransform(parentTransform, node);

    if (node.mesh >= 0) {
        const tinygltf::Mesh &mesh = model.meshes[node.mesh];
        logProgress("Processing Mesh: " + mesh.name);
        for (size_t i = 0; i < mesh.primitives.size(); i++) {
            sceneManager.modelPaths.push_back(mesh.name + " - " + std::to_string(i));
            sceneManager.meshTransforms.push_back(nodeTransform);
        }
    }

    if (node.camera >= 0) {
        const tinygltf::Camera &camera = model.cameras[node.camera];
        sceneManager.addCamera(node.name, camera, nodeTransform);
    }

    for (size_t i = 0; i < node.children.size(); i++) {
        processNode(node.children[i], nodeTransform, model, sceneManager);
    }
}

// -----------------------------------------------------------------------------
// Process all nodes from all scenes in the glTF model
// -----------------------------------------------------------------------------
static void processNodes(const tinygltf::Model &model, Vulkan::SceneManager &sceneManager) {
    logProgress("Processing Nodes...");
    // Default transform (as in your original code) with a zeroed translation/rotation and a scale of 1.
    glm::mat3 def(0.0f);
    def[2] = glm::vec3(1.0f, 1.0f, 1.0f); // assuming column 2 is used for scale.
    for (const auto &scene : model.scenes) {
        logProgress("Processing Scene: " + scene.name);
        for (const int nodeIndex : scene.nodes)
            processNode(nodeIndex, def, model, sceneManager);
    }
}

// -----------------------------------------------------------------------------
// Load triangles from a glTF primitive
// -----------------------------------------------------------------------------
static void loadTrianglesFromPrimitive(const tinygltf::Model &model, const tinygltf::Primitive &primitive, std::vector<VKPT::Triangle> &allTriangles) {
    auto pos_it  = primitive.attributes.find("POSITION");
    auto norm_it = primitive.attributes.find("NORMAL");
    if (pos_it == primitive.attributes.end() || norm_it == primitive.attributes.end()) {
        std::cerr << "Primitive missing POSITION or NORMAL, skipping." << std::endl;
        return;
    }

    const tinygltf::Accessor &posAccessor = model.accessors[pos_it->second];
    const float* positions = getAccessorData<float>(model, posAccessor);

    const tinygltf::Accessor &normAccessor = model.accessors[norm_it->second];
    const float* normals = getAccessorData<float>(model, normAccessor);

    const float* tangents = nullptr;
    if (primitive.attributes.find("TANGENT") != primitive.attributes.end())
        tangents = getAccessorData<float>(model, model.accessors[primitive.attributes.find("TANGENT")->second]);

    const float* uvs0 = nullptr;
    if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end())
        uvs0 = getAccessorData<float>(model, model.accessors[primitive.attributes.find("TEXCOORD_0")->second]);

    // Gather indices – if not provided, assume sequential indices.
    std::vector<unsigned int> indices;
    if (primitive.indices >= 0) {
        const tinygltf::Accessor &indexAccessor = model.accessors[primitive.indices];
        indices.resize(indexAccessor.count);
        const void* indexData = getAccessorData<unsigned char>(model, indexAccessor);
        if (indexAccessor.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT) {
            const unsigned short* buf = reinterpret_cast<const unsigned short*>(indexData);
            for (size_t i = 0; i < indexAccessor.count; i++)
                indices[i] = static_cast<unsigned int>(buf[i]);
        } else if (indexAccessor.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT) {
            const unsigned int* buf = reinterpret_cast<const unsigned int*>(indexData);
            std::memcpy(indices.data(), buf, indexAccessor.count * sizeof(unsigned int));
        } else {
            std::cerr << "Unsupported index type: " << indexAccessor.componentType << std::endl;
            return;
        }
    } else {
        indices.resize(posAccessor.count);
        for (size_t i = 0; i < posAccessor.count; ++i)
            indices[i] = static_cast<unsigned int>(i);
    }

    size_t numTriangles = indices.size() / 3;
    size_t prevSize = allTriangles.size();
    allTriangles.reserve(prevSize + numTriangles);
    for (size_t i = 0; i < numTriangles; i++) {
        VKPT::Triangle tri;
        for (int v = 0; v < 3; v++) {
            unsigned int idx = indices[i * 3 + v];
            std::memcpy(glm::value_ptr(tri.pos[v]), positions + idx * 3, sizeof(float) * 3);
            std::memcpy(glm::value_ptr(tri.norm[v]), normals + idx * 3, sizeof(float) * 3);
            if (tangents)
                std::memcpy(glm::value_ptr(tri.tan[v]), tangents + idx * 4, sizeof(float) * 4);
            if (uvs0)
                std::memcpy(glm::value_ptr(tri.uv0[v]), uvs0 + idx * 2, sizeof(float) * 2);
        }
        allTriangles.push_back(tri);
    }
}

// -----------------------------------------------------------------------------
// Process triangles from all primitives in all meshes
// -----------------------------------------------------------------------------
static void processTriangles(const tinygltf::Model &model, std::vector<VKPT::Triangle> &allTriangles) {
    logProgress("Processing Triangles...");
    for (const auto &mesh : model.meshes)
        for (const auto &primitive : mesh.primitives)
            loadTrianglesFromPrimitive(model, primitive, allTriangles);
}

// -----------------------------------------------------------------------------
// Process mesh primitives and build their BVH structures.
// Each primitive becomes its own mesh.
// -----------------------------------------------------------------------------
static void processMeshes(const tinygltf::Model &model, uint32_t triStartIndex, Vulkan::SceneManager &sceneManager) {
    logProgress("Processing Meshes and Building BVHs...");
    for (const auto &gltfMesh : model.meshes) {
        for (const auto &primitive : gltfMesh.primitives) {
            auto pos_it = primitive.attributes.find("POSITION");
            auto norm_it = primitive.attributes.find("NORMAL");
            if (pos_it == primitive.attributes.end() || norm_it == primitive.attributes.end())
                continue;

            uint32_t primTriangleCount = 0;
            if (primitive.indices >= 0) {
                const tinygltf::Accessor &indexAccessor = model.accessors[primitive.indices];
                primTriangleCount = static_cast<uint32_t>(indexAccessor.count / 3);
            } else {
                primTriangleCount = static_cast<uint32_t>(model.accessors[pos_it->second].count / 3);
            }

            VKPT::Mesh mesh;
            mesh.materialIndex = primitive.material;
            mesh.rootBVHNode = sceneManager.sceneData.numBVHs;
            if (sceneManager.sceneData.numMeshes >= MAX_MESHES)
                throw std::runtime_error("ERROR: Increase MAX_MESHES in Constants.h");
            sceneManager.sceneData.meshes[sceneManager.sceneData.numMeshes++] = mesh;

            VKPT::BVH root;
            root.triangleCount = primTriangleCount;
            root.index = triStartIndex;
            BVH::createBVH(mesh, root, sceneManager);

            triStartIndex += primTriangleCount;
        }
    }
}

// -----------------------------------------------------------------------------
// Main function to load a glTF file using a mat3 transform scheme
// -----------------------------------------------------------------------------
void loadGLTF(const std::string &filename, const glm::mat3 &transform, uint32_t matIndex, Vulkan::SceneManager &sceneManager) {
    if (filename.empty())
        return;

    logProgress("Loading model: " + filename);
    tinygltf::TinyGLTF loader;
    bool isBinary = (filename.find(".glb") != std::string::npos);
    tinygltf::Model model;
    std::string error, warning;
    bool ret = isBinary ? loader.LoadBinaryFromFile(&model, &error, &warning, filename)
                        : loader.LoadASCIIFromFile(&model, &error, &warning, filename);
    if (!warning.empty())
        std::cout << "Warning: " << warning << std::endl;
    if (!error.empty())
        std::cerr << "Error: " << error << std::endl;
    if (!ret) {
        std::cerr << "Failed to parse glTF file: " << filename << std::endl;
        return;
    }
    logProgress("Model loaded successfully!");

    uint32_t textureOffset = sceneManager.sceneData.numTextures;
    processMaterials(model, sceneManager, textureOffset);
    processTextures(extractDirectory(filename), model, sceneManager);
    processNodes(model, sceneManager);

    std::vector<VKPT::Triangle> allTriangles;
    processTriangles(model, allTriangles);

    uint32_t triStartIndex = sceneManager.sceneData.numTriangles;
    for (size_t i = 0; i < allTriangles.size(); i++) {
        sceneManager.triIndices.push_back(sceneManager.sceneData.numTriangles);
        glm::vec3 p0(allTriangles[i].pos[0]), p1(allTriangles[i].pos[1]), p2(allTriangles[i].pos[2]);
        sceneManager.triMin.push_back(glm::min(p0, glm::min(p1, p2)));
        sceneManager.triMax.push_back(glm::max(p0, glm::max(p1, p2)));
        sceneManager.triCentroid.push_back((p0 + p1 + p2) / 3.f);
        sceneManager.sceneData.numTriangles++;
    }

    if (sceneManager.sceneData.numTriangles >= MAX_TRIANGLES) {
        printf("Triangle count %d exceeds capacity %d!\n", sceneManager.sceneData.numTriangles, MAX_TRIANGLES);
        throw std::runtime_error("ERROR: Increase MAX_TRIANGLES in Constants.h");
    }

    processMeshes(model, triStartIndex, sceneManager);

    for (uint32_t i = triStartIndex; i < sceneManager.sceneData.numTriangles; i++) {
        sceneManager.sceneStorage->triangles[i] = allTriangles[sceneManager.triIndices[i] - triStartIndex];
    }

    sceneManager.updateMeshTransforms();
    sceneManager.uploadFullSceneStorage();
    sceneManager.resetAccumulation();
    logProgress("Finished loading model!");
}

} // namespace File
