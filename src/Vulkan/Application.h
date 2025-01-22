#pragma once

#include "CommandBuffer.h"
#include "CommandPool.h"
#include "DescriptorPool.h"
#include "DescriptorSet.h"
#include "Device.h"
#include "Fence.h"
#include "Framebuffer.h"
#include "GraphicsPipeline.h"
#include "Instance.h"
#include "Semaphore.h"
#include "Surface.h"
#include "SwapChain.h"
#include "Uniform.h"
#include "VulkanApp.h"
#include "Window.h"

namespace Vulkan {

class Application {
  public:
    NO_COPY(Application);

    Application();
    ~Application();

    void run();
    void onResize();
    void drawFrame();

  private:
    /* ----------------------- */
    uint32_t MAX_FRAMES_IN_FLIGHT = 1;
    uint32_t currentFrame = 0;
    uint32_t framesRendered = 0;
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
    DescriptorPool m_DescriptorPool;
    std::vector<DescriptorSet> m_DescriptorSets;
    std::vector<Uniform> m_Uniforms;
    ImageView m_AccumulationImageView;
    /* ----------------------- */
};

} // namespace Vulkan