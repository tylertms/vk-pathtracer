#include "GLTFLoader.h"
#include "FilePicker.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

namespace File {

GLTFLoader::GLTFLoader(VKPT_FILE_TYPE type) : GLTFLoader(pickFilePath(type, VKPT_LOAD)) {}

GLTFLoader::GLTFLoader(const std::string &filename) {
    if (filename.empty())
        return;

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

            for (size_t i = 0; i + 2 < indices.size(); i += 3) {
                unsigned int indexA = indices[i];
                unsigned int indexB = indices[i + 1];
                unsigned int indexC = indices[i + 2];

                VKPT::Triangle triangle;

                triangle.posA.x = positions[3 * indexA];
                triangle.posA.y = positions[3 * indexA + 1];
                triangle.posA.z = positions[3 * indexA + 2];

                triangle.posB.x = positions[3 * indexB];
                triangle.posB.y = positions[3 * indexB + 1];
                triangle.posB.z = positions[3 * indexB + 2];

                triangle.posC.x = positions[3 * indexC];
                triangle.posC.y = positions[3 * indexC + 1];
                triangle.posC.z = positions[3 * indexC + 2];

                triangle.normA.x = normals[3 * indexA];
                triangle.normA.y = normals[3 * indexA + 1];
                triangle.normA.z = normals[3 * indexA + 2];

                triangle.normB.x = normals[3 * indexB];
                triangle.normB.y = normals[3 * indexB + 1];
                triangle.normB.z = normals[3 * indexB + 2];

                triangle.normC.x = normals[3 * indexC];
                triangle.normC.y = normals[3 * indexC + 1];
                triangle.normC.z = normals[3 * indexC + 2];

                m_Triangles.push_back(triangle);
            }

            VKPT::Mesh meshStruct;
            m_Meshes.push_back(meshStruct);
        }
    }
}

bool GLTFLoader::LoadModel(const std::string &filename) {
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

} // namespace File
