#pragma once

#include "../../Vulkan/SceneManager.h"

namespace Interface {

void processCameraMovement(Vulkan::SceneManager &sceneManager);
void drawCameraControl(Vulkan::SceneManager &sceneManager);

} // namespace Interface
