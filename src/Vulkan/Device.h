#pragma once

#include "VkApp.h"
#include <vulkan/vulkan_beta.h>

#include <optional>
#include <vector>

namespace VKAPP {

const std::vector<const char *> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME};

class Device {
  public:
    void init(const VkInstance &instance, const VkSurfaceKHR &surface);
    void deinit();

    void pickPhysicalDevice();
    void createLogicalDevice();

  private:
    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete() {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };

  private:
    VkDevice m_Device;
    VkPhysicalDevice m_PhysicalDevice;

    VkQueue graphicsQueue;
    VkQueue presentQueue;

    QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice &device);
    int getScore(const VkPhysicalDevice &device, VkPhysicalDeviceProperties &properties);
    bool deviceSupportsExtensions(const VkPhysicalDevice &physicalDevice);
    const char *deviceString(const VkPhysicalDeviceType &type);

  private:
    VkInstance ext_Instance;
    VkSurfaceKHR ext_Surface;
};

} // namespace VKAPP