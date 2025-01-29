#include "Matrix.h"

namespace VKPT {

glm::mat4 computeInverseMatrix(const glm::vec3 &translation, const glm::vec3 &rotationEuler, const glm::vec3 &scale) {
    glm::mat4 mScale = glm::scale(glm::mat4(1.0f), scale);
    glm::mat4 mRotation = glm::mat4(glm::quat(glm::radians(rotationEuler)));
    glm::mat4 mTranslation = glm::translate(glm::mat4(1.0f), translation);
    glm::mat4 mTransform = mTranslation * mRotation * mScale;
    glm::mat4 mInv = glm::inverse(mTransform);

    return mInv;
}

glm::mat4 computeInverseMatrix(const glm::mat3 matrix) {
    return computeInverseMatrix(matrix[0], matrix[1], matrix[2]);
}


} // namespace VKPT
