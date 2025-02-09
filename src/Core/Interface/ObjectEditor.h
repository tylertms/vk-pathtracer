#pragma once

#include <glm/glm.hpp>

#include "../Types/Scene.h"
#include "../../Vulkan/SceneManager.h"

namespace Interface {

void drawObjectControl(Vulkan::SceneManager &sceneManager);

bool drawSphereEditor(VKPT::Sphere &sphere);
bool drawMeshEditor(VKPT::Mesh &mesh, glm::mat3 &transform);

} // namespace Interface
