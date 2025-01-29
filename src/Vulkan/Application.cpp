#include "Application.h"
#include "vulkan/vulkan_core.h"

#include <stdexcept>
#include <sys/_types/_dev_t.h>

namespace Vulkan {

Application::Application() {
    GLFWwindow *window = m_Window.init();
    VkInstance instance = m_Instance.init();

    VkSurfaceKHR surface = m_Surface.init(instance, window);
    VkDevice device = m_Device.init(instance, surface);

    m_SwapChain.init(m_Device, surface, window);

    m_DescriptorSets = std::vector<DescriptorSet>(MAX_FRAMES_IN_FLIGHT);
    for (uint32_t i = 0; i < m_DescriptorSets.size(); i++) {
        m_DescriptorSets[i].createLayout(m_Device.getVkDevice());
    }

    m_GraphicsPipeline.init(device, m_DescriptorSets.front().getVkDescriptorSetLayout(), m_SwapChain.getFormat(), m_SwapChain.getExtent());

    m_Framebuffers = std::vector<Framebuffer>(m_SwapChain.getImageCount());
    for (uint32_t i = 0; i < m_Framebuffers.size(); i++) {
        m_Framebuffers[i].init(device, m_GraphicsPipeline.getVkRenderPass(), m_SwapChain.getVkImageView(i), m_SwapChain.getExtent());
    }

    m_CommandPool.init(m_Device);

    m_DescriptorPool.init(m_Device.getVkDevice(), MAX_FRAMES_IN_FLIGHT);
    m_ImGuiDescriptorPool.initImGui(m_Device.getVkDevice(), MAX_FRAMES_IN_FLIGHT);

    m_AccumulationImageView.createImage(m_Device, m_SwapChain.getExtent(), VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_STORAGE_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    m_AccumulationImageView.transitionImageLayout(m_Device, m_CommandPool, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
    m_AccumulationImageView.init(m_Device.getVkDevice(), nullptr, VK_FORMAT_UNDEFINED);

    m_SceneManager.init(m_Device);

    m_CommandBuffers = std::vector<CommandBuffer>(MAX_FRAMES_IN_FLIGHT);
    m_ImageAvailableSemaphores = std::vector<Semaphore>(MAX_FRAMES_IN_FLIGHT);
    m_RenderFinishedSemaphores = std::vector<Semaphore>(MAX_FRAMES_IN_FLIGHT);
    m_InFlightFences = std::vector<Fence>(MAX_FRAMES_IN_FLIGHT);

    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        m_CommandBuffers[i].init(m_Device, m_CommandPool);
        m_DescriptorSets[i].createSet(m_Device.getVkDevice(), m_SceneManager, m_AccumulationImageView, m_DescriptorPool.getVkDescriptorPool());
        m_ImageAvailableSemaphores[i].init(device);
        m_RenderFinishedSemaphores[i].init(device);
        m_InFlightFences[i].init(device, true);
    }

    m_Interface.init(m_Device, m_Instance, m_Window, m_ImGuiDescriptorPool, m_SwapChain, m_GraphicsPipeline);

    m_SceneManager.sceneData.camera.aspectRatio = m_Window.getAspectRatio();
}

Application::~Application() {
    VkDevice device = m_Device.getVkDevice();

    m_Interface.deinit();

    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        m_ImageAvailableSemaphores[i].deinit(device);
        m_RenderFinishedSemaphores[i].deinit(device);
        m_InFlightFences[i].deinit(device);
    }

    for (uint32_t i = 0; i < m_Framebuffers.size(); i++) {
        m_Framebuffers[i].deinit(device);
    }

    m_SceneManager.deinit(device);
    m_AccumulationImageView.deinit(device);

    m_CommandPool.deinit(m_Device);
    m_GraphicsPipeline.deinit(device);

    m_DescriptorPool.deinit(device);
    m_ImGuiDescriptorPool.deinit(device);

    for (uint32_t i = 0; i < m_DescriptorSets.size(); i++) {
        m_DescriptorSets[i].deinit(device);
    }

    m_SwapChain.deinit(device);
    m_Device.deinit();
    m_Surface.deinit();
    m_Instance.deinit();
    m_Window.deinit();
}

void Application::run() {
    GLFWwindow *window = m_Window.getGlfwWindow();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        drawFrame();
    }

    m_Device.waitIdle();
}

void Application::onResize() {
    GLFWwindow *window = m_Window.getGlfwWindow();
    int width = 0, height = 0;

    glfwGetFramebufferSize(window, &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(window, &width, &height);
        glfwWaitEvents();
    }

    m_Window.setWindowSize(width, height);

    m_Device.waitIdle();

    /* ---------- DEINIT ---------- */
    for (uint32_t i = 0; i < m_Framebuffers.size(); i++)
        m_Framebuffers[i].deinit(m_Device.getVkDevice());

    m_AccumulationImageView.deinit(m_Device.getVkDevice());
    m_SwapChain.deinit(m_Device.getVkDevice());

    m_DescriptorPool.deinit(m_Device.getVkDevice());
    for (uint32_t i = 0; i < m_DescriptorSets.size(); i++)
        m_DescriptorSets[i].deinit(m_Device.getVkDevice());
    /* ---------- END DEINIT ---------- */

    /* ---------- REINIT ---------- */
    m_DescriptorPool.init(m_Device.getVkDevice(), MAX_FRAMES_IN_FLIGHT);
    for (uint32_t i = 0; i < m_DescriptorSets.size(); i++)
        m_DescriptorSets[i].createLayout(m_Device.getVkDevice());

    m_SwapChain.init(m_Device, m_Surface.getVkSurface(), window);
    m_GraphicsPipeline.updateExtent(m_SwapChain.getExtent());

    m_AccumulationImageView.createImage(m_Device, m_SwapChain.getExtent(), VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_STORAGE_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    m_AccumulationImageView.transitionImageLayout(m_Device, m_CommandPool, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
    m_AccumulationImageView.init(m_Device.getVkDevice(), nullptr, VK_FORMAT_UNDEFINED);

    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        m_DescriptorSets[i].createSet(m_Device.getVkDevice(), m_SceneManager, m_AccumulationImageView, m_DescriptorPool.getVkDescriptorPool());

    for (uint32_t i = 0; i < m_Framebuffers.size(); i++)
        m_Framebuffers[i].init(m_Device.getVkDevice(), m_GraphicsPipeline.getVkRenderPass(), m_SwapChain.getVkImageView(i), m_SwapChain.getExtent());
    /* ---------- END REINIT ---------- */
    m_SceneManager.sceneData.framesRendered = 0;
    m_SceneManager.sceneData.camera.aspectRatio = (float)m_Extent.x / m_Extent.y;
    m_SceneManager.resetAccumulation();
}

void Application::drawFrame() {
    vkWaitForFences(m_Device.getVkDevice(), 1, &m_InFlightFences[m_CurrentFrame].getVkFence(), VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(m_Device.getVkDevice(), m_SwapChain.getVkSwapChain(), UINT64_MAX, m_ImageAvailableSemaphores[m_CurrentFrame].getVkSemaphore(), VK_NULL_HANDLE, &imageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        onResize();
        return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("ERROR: Failed to acquire swapchain image.");
    }

    m_SceneManager.submitUniformUpdates();
    m_SceneManager.submitStorageUpdatesIfNeeded();
    m_SceneManager.sceneData.framesRendered++;

    vkResetFences(m_Device.getVkDevice(), 1, &m_InFlightFences[m_CurrentFrame].getVkFence());

    if (m_Extent.x == 0 && m_Extent.y == 0) {
        vkResetCommandBuffer(m_CommandBuffers[m_CurrentFrame].getVkCommandBuffer(), 0);
        m_CommandBuffers[m_CurrentFrame].record(m_GraphicsPipeline, m_SceneManager, m_Interface, m_Framebuffers[imageIndex].getVkFramebuffer(), m_DescriptorSets[m_CurrentFrame].getVkDescriptorSet(), m_Window, m_Position, m_Extent);
    }

    ImVec2 previousPosition = m_Position;
    ImVec2 previousExtent = m_Extent;

    vkResetCommandBuffer(m_CommandBuffers[m_CurrentFrame].getVkCommandBuffer(), 0);
    m_CommandBuffers[m_CurrentFrame].record(m_GraphicsPipeline, m_SceneManager, m_Interface, m_Framebuffers[imageIndex].getVkFramebuffer(), m_DescriptorSets[m_CurrentFrame].getVkDescriptorSet(), m_Window, m_Position, m_Extent);

    bool positionChanged = m_Position.x != previousPosition.x || m_Position.y != previousPosition.y;
    bool extentChanged = m_Extent.x != previousExtent.x || m_Extent.y != previousExtent.y;

    if (positionChanged || extentChanged) {
        m_SceneManager.sceneData.camera.aspectRatio = (float)m_Extent.x / m_Extent.y;
        m_SceneManager.resetAccumulation();
    }

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {m_ImageAvailableSemaphores[m_CurrentFrame].getVkSemaphore()};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_CommandBuffers[m_CurrentFrame].getVkCommandBuffer();

    VkSemaphore signalSemaphores[] = {m_RenderFinishedSemaphores[m_CurrentFrame].getVkSemaphore()};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(m_Device.getGraphicsQueue(), 1, &submitInfo, m_InFlightFences[m_CurrentFrame].getVkFence()) != VK_SUCCESS) {
        throw std::runtime_error("ERROR: Failed to submit draw command buffer.");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {m_SwapChain.getVkSwapChain()};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(m_Device.getPresentQueue(), &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_Window.wasResized()) {
        m_Window.clearResized();
        onResize();
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("ERROR: Failed to present swapchain image.");
    }

    m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

} // namespace Vulkan
