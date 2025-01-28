#pragma once

#include "../../Vulkan/DescriptorPool.h"
#include "../../Vulkan/Device.h"
#include "../../Vulkan/GraphicsPipeline.h"
#include "../../Vulkan/Instance.h"
#include "../../Vulkan/SceneManager.h"
#include "../../Vulkan/VulkanApp.h"
#include "../../Vulkan/Window.h"

#include "../../External/ImGui/backends/imgui_impl_glfw.h"
#include "../../External/ImGui/backends/imgui_impl_vulkan.h"
#include "../../External/ImGui/imgui.h"

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

    void draw(Vulkan::SceneManager &sceneManager, ImVec2 &position, ImVec2 &extent);

  private:
    bool m_ShowSceneControl = true;
    bool m_ShowStats = true;

    std::vector<float> m_FrameTimes;
    std::chrono::high_resolution_clock::time_point m_TimeStart, m_TimeCurrent;

    void drawStats(Vulkan::SceneManager &sceneManager);
    void drawMenuBar(Vulkan::SceneManager &sceneManager);
    void setupDockspace();
    bool drawSceneControl(Vulkan::SceneManager &sceneManager);
    bool drawSphereControl(VKPT::Sphere &sphere);
    bool drawMeshControl(VKPT::Mesh &mesh);
};

} // namespace Interface
