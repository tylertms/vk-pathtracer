#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

namespace VKPT {

void computeInverseMatrix(glm::mat4 &worldLocalTransform, glm::mat4 &localWorldTransform,
    const glm::vec3 &translation, const glm::vec3 &rotationEuler, const glm::vec3 &scale);

void computeInverseMatrix(glm::mat4 &worldLocalTransform, glm::mat4 &localWorldTransform, const glm::mat3 &matrix);

}
