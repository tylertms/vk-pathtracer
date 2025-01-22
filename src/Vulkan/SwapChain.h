#pragma once

#include "Framebuffer.h"
#include "ImageView.h"
#include "VulkanApp.h"

#include <vector>

namespace Vulkan {

class Device;

class SwapChain {
  public:
    NO_COPY(SwapChain);
    SwapChain() = default;
    
    VkSwapchainKHR init(Device &device, const VkSurfaceKHR &surface, GLFWwindow *window);
    void deinit(const VkDevice &device);

    struct SupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    SupportDetails querySupport(const VkPhysicalDevice &physicalDevice, const VkSurfaceKHR &surface);

    inline const VkSwapchainKHR &getVkSwapChain() const { return m_SwapChain; }
    inline const VkFormat &getFormat() const { return m_Format; }
    inline const VkExtent2D &getExtent() const { return m_Extent; }
    inline const uint32_t getImageCount() const { return m_ImageCount; }
    inline const VkImageView &getVkImageView(int i) const { return m_ImageViews[i].getVkImageView(); }

  private:
    VkSwapchainKHR m_SwapChain;

    VkSurfaceFormatKHR m_SurfaceFormat;
    VkPresentModeKHR m_PresentMode;
    VkExtent2D m_Extent;
    VkFormat m_Format;

    uint32_t m_ImageCount;

    std::vector<VkImage> m_Images;
    std::vector<ImageView> m_ImageViews;

    VkSurfaceFormatKHR chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
    VkPresentModeKHR choosePresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
    VkExtent2D chooseExtent(const VkSurfaceCapabilitiesKHR &capabilities, GLFWwindow *window);
    VkSwapchainCreateInfoKHR getCreateInfo(Device &device, const VkSurfaceKHR &surface);
};

} // namespace Vulkan
