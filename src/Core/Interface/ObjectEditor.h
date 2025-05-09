#pragma once

#include <glm/glm.hpp>

#include "../../Vulkan/SceneManager.h"

namespace Interface {

void drawSceneOverview(Vulkan::SceneManager &sceneManager);
void drawObjectEditor(Vulkan::SceneManager &sceneManager);

bool drawSphereEditor(VKPT::Sphere &sphere);
bool drawMeshEditor(VKPT::Mesh &mesh, glm::mat3 &transform);

} // namespace Interface
