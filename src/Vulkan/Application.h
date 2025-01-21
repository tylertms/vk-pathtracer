#pragma once

#include "Device.h"
#include "Instance.h"
#include "GraphicsPipeline.h"
#include "Surface.h"
#include "SwapChain.h"
#include "Window.h"

namespace Vulkan {

class Application {
  public:
    Application();
    ~Application();

    void run();

    const Window &getWindow() const { return m_Window; }
    const Instance &getInstance() const { return m_Instance; }
    const Device &getDevice() const { return m_Device; }
    const Surface &getSurface() const { return m_Surface; }
    const SwapChain &getSwapChain() const { return m_SwapChain; }
    const GraphicsPipeline &getGraphicsPipeline() const { return m_GraphicsPipeline; }

  private:
    Window m_Window;
    Instance m_Instance;
    Device m_Device;
    Surface m_Surface;
    SwapChain m_SwapChain;
    GraphicsPipeline m_GraphicsPipeline;
};

extern Application g_AppInstance;

} // namespace Vulkan