#pragma once

#include "CommandPool.h"

namespace Vulkan {

void createTextureImage(const std::string filename, ImageView &textureImageView, const Device &device, const CommandPool &commandPool);

}
