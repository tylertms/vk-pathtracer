#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

glm::mat4 createTransformationMatrix(
    const glm::vec3& scaleFactors,
    const glm::vec3& rotationDegrees,
    const glm::vec3& translation
);

glm::vec3 applyTransformation(
    const glm::vec3& vec,
    const glm::mat4& transform
);
