#pragma once

#include "SwapChain.h"
#include "VulkanApp.h"

#include <vulkan/vulkan_beta.h>

#include <optional>
#include <vector>

namespace Vulkan {

const std::vector<const char *> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME};

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
    void init(const VkInstance &instance, const VkSurfaceKHR &surface);
    void deinit();

    void pickPhysicalDevice();
    void createLogicalDevice();

    const VkPhysicalDevice &getVkPhysicalDevice() const { return m_PhysicalDevice; }
    const VkDevice &getVkDevice() const { return m_Device; }

    QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice &physicalDevice, const VkSurfaceKHR &surface);

  private:
    VkDevice m_Device;
    VkPhysicalDevice m_PhysicalDevice;

    VkQueue graphicsQueue;
    VkQueue presentQueue;

    SwapChain m_SwapChain;

    int getScore(const VkPhysicalDevice &device, VkPhysicalDeviceProperties &properties);
    bool deviceSupportsExtensions(const VkPhysicalDevice &physicalDevice);
    const char *deviceString(const VkPhysicalDeviceType &type);

  private:
    VkInstance ext_Instance;
    VkSurfaceKHR ext_Surface;
};

} // namespace Vulkan