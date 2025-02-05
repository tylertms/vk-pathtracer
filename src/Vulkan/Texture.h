#pragma once

#include "CommandPool.h"

#include <cstring>
#include <string>

namespace Vulkan {

void createTextureImage(const std::string filename, ImageView &textureImageView, const Device &device, const CommandPool &commandPool);

}
