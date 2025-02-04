#pragma once

#include "CommandPool.h"

namespace Vulkan {

void createTextureImage(ImageView &textureImageView, const Device &device, const CommandPool &commandPool);

}
