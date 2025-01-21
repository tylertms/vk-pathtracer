#pragma once

#include "VkApp.h"

#include <optional>

namespace VKAPP {

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() { return graphicsFamily.has_value() /*&& presentFamily.has_value()*/; }
};

class Device {
  public:
    void init(const VkInstance &instance);
    ~Device();

    void pickPhysicalDevice(const VkInstance &instance);

  private:
    VkPhysicalDevice m_PhysicalDevice;

    int getScore(const VkPhysicalDevice &device, VkPhysicalDeviceProperties &properties);
};

} // namespace VKAPP