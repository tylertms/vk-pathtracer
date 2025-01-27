#include "Matrix.h"

namespace VKPT {

glm::mat4 createTransformationMatrix(
    const glm::vec3& scaleFactors,
    const glm::vec3& rotationDegrees,
    const glm::vec3& translation
) {
    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, translation);

    transform = glm::rotate(transform, glm::radians(rotationDegrees.z), glm::vec3(0, 0, 1));
    transform = glm::rotate(transform, glm::radians(rotationDegrees.y), glm::vec3(0, 1, 0));
    transform = glm::rotate(transform, glm::radians(rotationDegrees.x), glm::vec3(1, 0, 0));

    transform = glm::scale(transform, scaleFactors);

    return transform;
}

glm::vec3 applyTransformation(
    const glm::vec3& vec,
    const glm::mat4& transform
) {
    glm::vec4 homogenousVec = glm::vec4(vec, 1.0f);
    glm::vec4 transformedVec = transform * homogenousVec;
    return glm::vec3(transformedVec);
}

}
