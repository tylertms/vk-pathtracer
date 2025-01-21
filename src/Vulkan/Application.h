#pragma once

#include "CommandBuffer.h"
#include "CommandPool.h"
#include "Device.h"
#include "Fence.h"
#include "Framebuffer.h"
#include "Instance.h"
#include "GraphicsPipeline.h"
#include "Semaphore.h"
#include "Surface.h"
#include "SwapChain.h"
#include "Window.h"

namespace Vulkan {

class Application {
  public:
    Application();
    ~Application();

    void run();
    void drawFrame();

  private:
    Window m_Window;
    Instance m_Instance;
    Device m_Device;
    Surface m_Surface;
    SwapChain m_SwapChain;
    GraphicsPipeline m_GraphicsPipeline;

    std::vector<Framebuffer> m_Framebuffers;

    CommandPool m_CommandPool;
    CommandBuffer m_CommandBuffer;

    Semaphore m_ImageAvailableSemaphore;
    Semaphore m_RenderFinishedSemaphore;
    Fence m_InFlightFence;
};

extern Application g_AppInstance;

} // namespace Vulkan