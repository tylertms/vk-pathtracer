#pragma once

#include "CommandBuffer.h"
#include "CommandPool.h"
#include "Device.h"
#include "Fence.h"
#include "Framebuffer.h"
#include "GraphicsPipeline.h"
#include "Instance.h"
#include "Semaphore.h"
#include "Surface.h"
#include "SwapChain.h"
#include "VulkanApp.h"
#include "Window.h"

namespace Vulkan {

class Application {
  public:
    NO_COPY(Application);

    Application();
    ~Application();

    void run();
    void rebuild();
    void drawFrame();

  private:
    /* ----------------------- */
    uint32_t MAX_FRAMES_IN_FLIGHT = 2;
    uint32_t currentFrame = 0;
    /* ----------------------- */
    Window m_Window;
    Instance m_Instance;
    Device m_Device;
    Surface m_Surface;
    SwapChain m_SwapChain;
    GraphicsPipeline m_GraphicsPipeline;
    std::vector<Framebuffer> m_Framebuffers;
    CommandPool m_CommandPool;
    std::vector<CommandBuffer> m_CommandBuffers;
    std::vector<Semaphore> m_ImageAvailableSemaphores;
    std::vector<Semaphore> m_RenderFinishedSemaphores;
    std::vector<Fence> m_InFlightFences;
    /* ----------------------- */
};

} // namespace Vulkan