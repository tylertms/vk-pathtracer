#pragma once

#include "VkApp.h"

#include <optional>

namespace VKAPP {

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

class Device {
  public:
    void init(const VkInstance &instance, const VkSurfaceKHR &surface);
    void deinit();

    void pickPhysicalDevice();
    void createLogicalDevice();

  private:
    VkDevice m_Device;
    VkPhysicalDevice m_PhysicalDevice;

    VkQueue graphicsQueue;
    VkQueue presentQueue;

    QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice &device);
    int getScore(const VkPhysicalDevice &device, VkPhysicalDeviceProperties &properties);
    const char *deviceString(const VkPhysicalDeviceType &type);

  private:
    VkInstance ext_Instance;
    VkSurfaceKHR ext_Surface;
};

} // namespace VKAPP