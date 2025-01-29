#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

namespace VKPT {

glm::mat4 computeInverseMatrix(const glm::vec3 &translation, const glm::vec3 &rotationEuler, const glm::vec3 &scale);
glm::mat4 computeInverseMatrix(const glm::mat3 matrix);

}
