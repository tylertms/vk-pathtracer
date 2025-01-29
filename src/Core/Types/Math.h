#ifndef CORE_OBJECTS_VEC3_H
#define CORE_OBJECTS_VEC3_H

#include <glm/glm.hpp>
#include <yaml-cpp/yaml.h>

namespace YAML {
template <> struct convert<glm::vec3> {
    static Node encode(const glm::vec3 &rhs) {
        Node node;
        node.SetStyle(YAML::EmitterStyle::Flow);
        node.push_back(rhs.x);
        node.push_back(rhs.y);
        node.push_back(rhs.z);
        return node;
    }

    static bool decode(const Node &node, glm::vec3 &rhs) {
        if (!node.IsSequence() || node.size() != 3) {
        return false;
        }

        rhs.x = node[0].as<float>();
        rhs.y = node[1].as<float>();
        rhs.z = node[2].as<float>();
        return true;
    }
};

template <> struct convert<glm::mat4> {
    static Node encode(const glm::mat4 &rhs) {
        Node node;
        node.SetStyle(YAML::EmitterStyle::Flow);
        for (int col = 0; col < 4; ++col) {
        for (int row = 0; row < 4; ++row) {
            node.push_back(rhs[col][row]);
        }
        }
        return node;
    }

    static bool decode(const Node &node, glm::mat4 &rhs) {
        if (!node.IsSequence() || node.size() != 16) {
        return false;
        }

        for (int col = 0; col < 4; ++col) {
        for (int row = 0; row < 4; ++row) {
            int index = col * 4 + row;
            if (!node[index].IsScalar()) {
            return false;
            }
            rhs[col][row] = node[index].as<float>();
        }
        }
        return true;
    }
};

template <> struct convert<glm::mat3> {
    static Node encode(const glm::mat3 &rhs) {
        Node node;
        node.SetStyle(YAML::EmitterStyle::Flow);
        for (int col = 0; col < 3; ++col) {
        for (int row = 0; row < 3; ++row) {
            node.push_back(rhs[col][row]);
        }
        }
        return node;
    }

    static bool decode(const Node &node, glm::mat3 &rhs) {
        if (!node.IsSequence() || node.size() != 9) {
        return false;
        }

        for (int col = 0; col < 3; ++col) {
        for (int row = 0; row < 3; ++row) {
            int index = col * 3 + row;
            if (!node[index].IsScalar()) {
            return false;
            }
            rhs[col][row] = node[index].as<float>();
        }
        }
        return true;
  }
};
} // namespace YAML

#endif
