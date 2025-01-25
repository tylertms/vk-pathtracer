
#include "Loader.h"
#include <cstring>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

namespace Interface {

Loader::Loader(const std::string &filename) {
    if (!LoadModel(filename)) {
        std::cerr << "Failed to load glTF model: " << filename << std::endl;
        return;
    }

    for (const auto &mesh : m_Model.meshes) {
        for (const auto &primitive : mesh.primitives) {

            auto pos_it = primitive.attributes.find("POSITION");
            auto norm_it = primitive.attributes.find("NORMAL");

            if (pos_it == primitive.attributes.end() || norm_it == primitive.attributes.end()) {
                std::cerr << "Primitive missing POSITION or NORMAL attribute." << std::endl;
                continue;
            }

            const tinygltf::Accessor &posAccessor = m_Model.accessors[pos_it->second];
            const tinygltf::Accessor &normAccessor = m_Model.accessors[norm_it->second];

            const tinygltf::BufferView &posBufferView = m_Model.bufferViews[posAccessor.bufferView];
            const tinygltf::BufferView &normBufferView = m_Model.bufferViews[normAccessor.bufferView];

            const tinygltf::Buffer &posBuffer = m_Model.buffers[posBufferView.buffer];
            const tinygltf::Buffer &normBuffer = m_Model.buffers[normBufferView.buffer];

            const float *positions = reinterpret_cast<const float *>(
                posBuffer.data.data() + posBufferView.byteOffset + posAccessor.byteOffset);
            const float *normals = reinterpret_cast<const float *>(
                normBuffer.data.data() + normBufferView.byteOffset + normAccessor.byteOffset);

            std::vector<unsigned int> indices;
            if (primitive.indices >= 0) {
                const tinygltf::Accessor &indexAccessor = m_Model.accessors[primitive.indices];
                const tinygltf::BufferView &indexBufferView = m_Model.bufferViews[indexAccessor.bufferView];
                const tinygltf::Buffer &indexBuffer = m_Model.buffers[indexBufferView.buffer];

                if (indexAccessor.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT) {
                    const unsigned short *buf = reinterpret_cast<const unsigned short *>(
                        indexBuffer.data.data() + indexBufferView.byteOffset + indexAccessor.byteOffset);
                    indices.assign(buf, buf + indexAccessor.count);
                } else if (indexAccessor.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT) {
                    const unsigned int *buf = reinterpret_cast<const unsigned int *>(
                        indexBuffer.data.data() + indexBufferView.byteOffset + indexAccessor.byteOffset);
                    indices.assign(buf, buf + indexAccessor.count);
                } else {
                    std::cerr << "Unsupported index component type: " << indexAccessor.componentType << std::endl;
                    continue;
                }
            } else {

                indices.resize(posAccessor.count);
                for (unsigned int i = 0; i < posAccessor.count; ++i) {
                    indices[i] = i;
                }
            }

            uint32_t startIndex = static_cast<uint32_t>(m_Triangles.size());

            uint32_t triangleCount = static_cast<uint32_t>(indices.size() / 3);

            for (size_t i = 0; i + 2 < indices.size(); i += 3) {
                unsigned int indexA = indices[i];
                unsigned int indexB = indices[i + 1];
                unsigned int indexC = indices[i + 2];

                float posA[3] = {
                    positions[3 * indexA],
                    positions[3 * indexA + 1],
                    positions[3 * indexA + 2]};
                float posB[3] = {
                    positions[3 * indexB],
                    positions[3 * indexB + 1],
                    positions[3 * indexB + 2]};
                float posC[3] = {
                    positions[3 * indexC],
                    positions[3 * indexC + 1],
                    positions[3 * indexC + 2]};

                float normA[3] = {
                    normals[3 * indexA],
                    normals[3 * indexA + 1],
                    normals[3 * indexA + 2]};
                float normB[3] = {
                    normals[3 * indexB],
                    normals[3 * indexB + 1],
                    normals[3 * indexB + 2]};
                float normC[3] = {
                    normals[3 * indexC],
                    normals[3 * indexC + 1],
                    normals[3 * indexC + 2]};

                Vulkan::Triangle triangle;
                std::memcpy(triangle.posA, posA, sizeof(float) * 3);
                std::memcpy(triangle.posB, posB, sizeof(float) * 3);
                std::memcpy(triangle.posC, posC, sizeof(float) * 3);
                std::memcpy(triangle.normA, normA, sizeof(float) * 3);
                std::memcpy(triangle.normB, normB, sizeof(float) * 3);
                std::memcpy(triangle.normC, normC, sizeof(float) * 3);

                m_Triangles.push_back(triangle);
            }

            Vulkan::Mesh meshStruct;
            meshStruct.triangleCount = triangleCount;
            meshStruct.startIndex = startIndex;

            m_Meshes.push_back(meshStruct);
        }
    }
}

bool Loader::LoadModel(const std::string &filename) {
    tinygltf::TinyGLTF loader;
    bool isBinary = filename.find(".glb") != std::string::npos;

    bool ret;
    if (isBinary) {
        ret = loader.LoadBinaryFromFile(&m_Model, &m_ErrorMsg, &m_WarningMsg, filename);
    } else {
        ret = loader.LoadASCIIFromFile(&m_Model, &m_ErrorMsg, &m_WarningMsg, filename);
    }

    if (!m_WarningMsg.empty()) {
        std::cout << "Warning: " << m_WarningMsg << std::endl;
    }

    if (!m_ErrorMsg.empty()) {
        std::cerr << "Error: " << m_ErrorMsg << std::endl;
    }

    if (!ret) {
        std::cerr << "Failed to parse glTF file: " << filename << std::endl;
        return false;
    }

    return true;
}

} // namespace Interface
