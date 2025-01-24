#pragma once

#include "SwapChain.h"
#include "VulkanApp.h"
#include "vulkan/vulkan_core.h"

#include <vulkan/vulkan_beta.h>

#include <optional>
#include <vector>

namespace Vulkan {

const std::vector<const char *> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME,
    VK_EXT_FRAGMENT_SHADER_INTERLOCK_EXTENSION_NAME
};

class Device {
  public:
    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete() {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };

  public:
    NO_COPY(Device);
    Device() = default;

    VkDevice init(const VkInstance &instance, const VkSurfaceKHR &surface);
    void deinit();

    void waitIdle();

    void pickPhysicalDevice();
    void createLogicalDevice();

    inline const VkPhysicalDevice &getVkPhysicalDevice() const { return m_PhysicalDevice; }
    inline const VkDevice &getVkDevice() const { return m_Device; }

    inline const VkQueue &getGraphicsQueue() const { return m_GraphicsQueue; }
    inline const VkQueue &getPresentQueue() const { return m_PresentQueue; }

    QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice &physicalDevice, const VkSurfaceKHR &opt_Surface) const;

  private:
    VkDevice m_Device;
    VkPhysicalDevice m_PhysicalDevice;

    VkQueue m_GraphicsQueue;
    VkQueue m_PresentQueue;

    int getScore(const VkPhysicalDevice &device, VkPhysicalDeviceProperties &properties);
    bool deviceSupportsExtensions(const VkPhysicalDevice &physicalDevice);

    const char *deviceString(const VkPhysicalDeviceType &type);
    void logInfo(const VkPhysicalDeviceProperties &properties, int index);

  private:
    VkInstance ext_Instance;
    VkSurfaceKHR ext_Surface = nullptr;
};

} // namespace Vulkan
