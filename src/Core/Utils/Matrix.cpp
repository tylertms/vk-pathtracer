#include "Matrix.h"

namespace VKPT {

void computeInverseMatrix(glm::mat4 &worldLocal, glm::mat4 &localWorld, const glm::vec3 &translation, const glm::vec3 &rotationEuler, const glm::vec3 &scale) {
    glm::mat4 mScale = glm::scale(glm::mat4(1.0f), scale);
    glm::quat quatRotation = glm::normalize(glm::quat(glm::radians(rotationEuler)));
    glm::mat4 mRotation = glm::mat4(quatRotation);
    glm::mat4 mTranslation = glm::translate(glm::mat4(1.0f), translation);
    localWorld = mTranslation * mRotation * mScale;
    worldLocal = glm::inverse(localWorld);
}

void computeInverseMatrix(glm::mat4 &worldLocal, glm::mat4 &localWorld, const glm::mat3 matrix) {
    computeInverseMatrix(worldLocal, localWorld, matrix[0], matrix[1], matrix[2]);
}

} // namespace VKPT
