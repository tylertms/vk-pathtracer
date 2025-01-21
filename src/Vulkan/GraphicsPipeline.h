#pragma once

#include "VulkanApp.h"

namespace Vulkan {

class GraphicsPipeline {
  public:
    void init(const VkDevice &device);
    void deinit();
};

} // namespace Vulkan