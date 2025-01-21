#pragma once

#include "VulkanApp.h"

#include <vector>

namespace Vulkan {

class Device;

class SwapChain {
  public:
    void init(Device &device, const VkSurfaceKHR &surface, GLFWwindow *window);
    void deinit(const VkDevice &device);

    struct SupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    SupportDetails querySupport(const VkPhysicalDevice &physicalDevice, const VkSurfaceKHR &surface);

  private:
    VkSwapchainKHR m_SwapChain;

    VkSurfaceFormatKHR m_SurfaceFormat;
    VkPresentModeKHR m_PresentMode;
    VkExtent2D m_Extent;

    VkSurfaceFormatKHR chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
    VkPresentModeKHR choosePresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
    VkExtent2D chooseExtent(const VkSurfaceCapabilitiesKHR &capabilities, GLFWwindow *window);
    VkSwapchainCreateInfoKHR getCreateInfo(Device &device, const VkSurfaceKHR &surface);
};

} // namespace Vulkan
