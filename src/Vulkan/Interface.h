#pragma once

#include "DescriptorPool.h"
#include "Device.h"
#include "GraphicsPipeline.h"
#include "Instance.h"
#include "VulkanApp.h"
#include "Window.h"

namespace Vulkan {

class Interface {
  public:
    NO_COPY(Interface)
    Interface() = default;

    void init(const Device &device, const Instance &instance,
        const Window &window, const DescriptorPool &descriptorPool,
        const SwapChain &swapChain, const GraphicsPipeline &graphicsPipeline);

    void deinit();
};

}
