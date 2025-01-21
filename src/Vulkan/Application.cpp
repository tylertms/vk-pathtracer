#include "Application.h"
#include <iostream>

namespace Vulkan {

Application::Application() {
    m_GLFWwindow = m_Window.init();
    m_VkInstance = m_Instance.init();

    m_VkSurface = m_Surface.init(m_VkInstance, m_GLFWwindow);
    m_VkDevice = m_Device.init(m_VkInstance, m_VkSurface);
    m_VkSwapChain = m_SwapChain.init(m_Device, m_VkSurface, m_GLFWwindow);

    m_GraphicsPipeline.init(m_VkDevice, m_SwapChain.getFormat(), m_SwapChain.getExtent());

    m_Framebuffers.resize(m_SwapChain.getImageCount());

    for (int i = 0; i < m_Framebuffers.size(); i++) {
        m_Framebuffers[i].init(m_VkDevice, m_GraphicsPipeline.getVkRenderPass(), m_SwapChain.getVkImageView(i), m_SwapChain.getExtent());
    }

    m_CommandPool.init(m_Device);

    m_CommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    m_ImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_RenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_InFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        m_CommandBuffers[i].init(m_Device, m_GraphicsPipeline.getRenderPass(), m_CommandPool);
        m_ImageAvailableSemaphores[i].init(m_VkDevice);
        m_RenderFinishedSemaphores[i].init(m_VkDevice);
        m_InFlightFences[i].init(m_VkDevice, true);
    }
}

Application::~Application() {
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        m_ImageAvailableSemaphores[i].deinit(m_VkDevice);
        m_RenderFinishedSemaphores[i].deinit(m_VkDevice);
        m_InFlightFences[i].deinit(m_VkDevice);
    }

    m_CommandPool.deinit(m_Device);

    for (auto framebuffer : m_Framebuffers) {
        framebuffer.deinit(m_VkDevice);
    }

    m_GraphicsPipeline.deinit(m_VkDevice);
    m_SwapChain.deinit(m_VkDevice);
    m_Device.deinit();
    m_Surface.deinit();
    m_Instance.deinit();
    m_Window.deinit();
}

void Application::run() {
    while (!glfwWindowShouldClose(m_GLFWwindow)) {
        glfwPollEvents();
        drawFrame();
    }

    vkDeviceWaitIdle(m_VkDevice);
}

void Application::drawFrame() {
    vkWaitForFences(m_VkDevice, 1, &m_InFlightFences[currentFrame].getVkFence(), VK_TRUE, UINT64_MAX);
    vkResetFences(m_VkDevice, 1, &m_InFlightFences[currentFrame].getVkFence());

    uint32_t imageIndex;
    vkAcquireNextImageKHR(m_VkDevice, m_VkSwapChain, UINT64_MAX, m_ImageAvailableSemaphores[currentFrame].getVkSemaphore(), VK_NULL_HANDLE, &imageIndex);

    vkResetCommandBuffer(m_CommandBuffers[currentFrame].getVkCommandBuffer(), 0);
    m_CommandBuffers[currentFrame].record(m_GraphicsPipeline, m_Framebuffers[imageIndex].getVkFramebuffer());

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {m_ImageAvailableSemaphores[currentFrame].getVkSemaphore()};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_CommandBuffers[currentFrame].getVkCommandBuffer();

    VkSemaphore signalSemaphores[] = {m_RenderFinishedSemaphores[currentFrame].getVkSemaphore()};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(m_Device.getGraphicsQueue(), 1, &submitInfo, m_InFlightFences[currentFrame].getVkFence()) != VK_SUCCESS) {
        throw std::runtime_error("ERROR: Failed to submit draw command buffer.");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {m_VkSwapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;

    vkQueuePresentKHR(m_Device.getPresentQueue(), &presentInfo);

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

} // namespace Vulkan