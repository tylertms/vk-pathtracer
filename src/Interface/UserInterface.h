#pragma once

#include "../Vulkan/DescriptorPool.h"
#include "../Vulkan/Device.h"
#include "../Vulkan/GraphicsPipeline.h"
#include "../Vulkan/Instance.h"
#include "../Vulkan/Scene.h"
#include "../Vulkan/VulkanApp.h"
#include "../Vulkan/Window.h"

#include <chrono>

namespace Interface {

class UserInterface {
  public:
    NO_COPY(UserInterface)
    UserInterface() = default;

    void init(const Vulkan::Device &device, const Vulkan::Instance &instance,
                const Vulkan::Window &window,
                const Vulkan::DescriptorPool &descriptorPool,
                const Vulkan::SwapChain &swapChain,
                const Vulkan::GraphicsPipeline &graphicsPipeline);

    void deinit();
    void draw(Vulkan::Scene &scene);

  private:
    uint32_t m_DisplayFPS = 0;
    uint32_t m_FramesLastSecond = 0;
    std::chrono::steady_clock::time_point m_TimeStart, m_TimeCurrent;

    bool drawSphereControl(Sphere &sphere);
};

} // namespace Interface
