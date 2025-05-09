#include "CommandBuffer.h"

#include "../External/ImGui/backends/imgui_impl_vulkan.h"
#include "../External/ImGui/imgui.h"
#include "SceneManager.h"
#include "vulkan/vulkan_core.h"

#include <stdexcept>

namespace Vulkan {

void CommandBuffer::init(const Device &device, const CommandPool &commandPool) {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool.getVkCommandPool();
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(device.getVkDevice(), &allocInfo, &m_CommandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("ERROR: Failed to allocate command buffers.");
    }
}

void CommandBuffer::record(const GraphicsPipeline &graphicsPipeline, SceneManager &sceneManager, Interface::UserInterface &interface, const VkFramebuffer &framebuffer, const VkDescriptorSet &descriptorSet, const Window &window, ImVec2 &position, ImVec2 &extent) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    if (vkBeginCommandBuffer(m_CommandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("ERROR: Failed to begin recording command buffer.");
    }

    VkRenderPassBeginInfo renderPassInfo = graphicsPipeline.getRenderPass().getBeginInfo(framebuffer);

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(m_CommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline.getVkPipeline());

    float scaleX, scaleY;
    glfwGetWindowContentScale(window.getGlfwWindow(), &scaleX, &scaleY);

    VkViewport viewport;
    viewport.x = position.x * scaleX;
    viewport.y = position.y * scaleY;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    viewport.width = max(1.0f, extent.x * scaleX);
    viewport.height = max(1.0f, extent.y * scaleY);
    vkCmdSetViewport(m_CommandBuffer, 0, 1, &viewport);

    VkRect2D scissor = graphicsPipeline.getScissor();
    vkCmdSetScissor(m_CommandBuffer, 0, 1, &scissor);

    vkCmdBindDescriptorSets(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline.getVkPipelineLayout(), 0, 1, &descriptorSet, 0, nullptr);
    vkCmdDraw(m_CommandBuffer, 3, 1, 0, 0);

    interface.draw(sceneManager, position, extent);
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), m_CommandBuffer);

    vkCmdEndRenderPass(m_CommandBuffer);
    if (vkEndCommandBuffer(m_CommandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("ERROR: Failed to record command buffer.");
    }
}

} // namespace Vulkan
