#pragma once

#include "DescriptorPool.h"
#include "Device.h"
#include "GraphicsPipeline.h"
#include "Instance.h"
#include "Scene.h"
#include "VulkanApp.h"
#include "Window.h"

#include <chrono>

namespace Vulkan {

class Interface {
  public:
    NO_COPY(Interface)
    Interface() = default;

    void init(const Device &device, const Instance &instance,
              const Window &window, const DescriptorPool &descriptorPool,
              const SwapChain &swapChain, const GraphicsPipeline &graphicsPipeline);

    void deinit();
    void draw(Scene &scene);

  private:
    uint32_t m_DisplayFPS;
    uint32_t m_FramesLastSecond;
    std::chrono::steady_clock::time_point m_TimeStart, m_TimeCurrent;

    bool drawSphereControl(Sphere &sphere);
};

} // namespace Vulkan
