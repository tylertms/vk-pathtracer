#pragma once

#include "VulkanApp.h"
#include "ImageView.h"

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

    const VkFormat &getFormat() const { return m_Format; }
    const VkExtent2D &getExtent() const { return m_Extent; }

  private:
    VkSwapchainKHR m_SwapChain;

    VkSurfaceFormatKHR m_SurfaceFormat;
    VkPresentModeKHR m_PresentMode;
    VkExtent2D m_Extent;
    VkFormat m_Format;

    std::vector<VkImage> m_Images;
    std::vector<ImageView> m_ImageViews;

    VkSurfaceFormatKHR chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
    VkPresentModeKHR choosePresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
    VkExtent2D chooseExtent(const VkSurfaceCapabilitiesKHR &capabilities, GLFWwindow *window);
    VkSwapchainCreateInfoKHR getCreateInfo(Device &device, const VkSurfaceKHR &surface);
};

} // namespace Vulkan
