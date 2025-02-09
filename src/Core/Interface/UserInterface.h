#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include <vector>
#include <chrono>
#include <imgui.h>

#include "../../Vulkan/Device.h"
#include "../../Vulkan/Instance.h"
#include "../../Vulkan/Window.h"
#include "../../Vulkan/DescriptorPool.h"
#include "../../Vulkan/SwapChain.h"
#include "../../Vulkan/GraphicsPipeline.h"
#include "../../Vulkan/SceneManager.h"

namespace Interface {

class UserInterface {
  public:
    void init(
      const Vulkan::Device &device, 
      const Vulkan::Instance &instance,
      const Vulkan::Window &window, 
      const Vulkan::DescriptorPool &descriptorPool,
      const Vulkan::SwapChain &swapChain, 
      const Vulkan::GraphicsPipeline &graphicsPipeline
    );

    void deinit();
    void draw(Vulkan::SceneManager &sceneManager, ImVec2 &position, ImVec2 &extent);

  private:
    void drawStats(Vulkan::SceneManager &sceneManager);
    void drawMenuBar(Vulkan::SceneManager &sceneManager);
    void setupDockspace();

    bool m_ShowCameraControl = true;
    bool m_ShowObjectControl = true;
    bool m_ShowStats = true;

    std::chrono::high_resolution_clock::time_point m_TimeStart;
    std::chrono::high_resolution_clock::time_point m_TimeCurrent;
    std::vector<float> m_FrameTimes;

};

} // namespace Interface

#endif
