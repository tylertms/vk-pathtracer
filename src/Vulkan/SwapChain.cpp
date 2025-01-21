#include "SwapChain.h"

#include "Device.h"

namespace Vulkan {

VkSwapchainKHR SwapChain::init(Device &device, const VkSurfaceKHR &surface, GLFWwindow *window) {
    VkPhysicalDevice physicalDevice = device.getVkPhysicalDevice();
    SupportDetails swapChainSupport = querySupport(physicalDevice, surface);

    m_ImageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && m_ImageCount > swapChainSupport.capabilities.maxImageCount) {
        m_ImageCount = swapChainSupport.capabilities.maxImageCount;
    }

    m_SurfaceFormat = chooseSurfaceFormat(swapChainSupport.formats);
    m_PresentMode = choosePresentMode(swapChainSupport.presentModes);
    m_Extent = chooseExtent(swapChainSupport.capabilities, window);
    m_Format = m_SurfaceFormat.format;

    VkSwapchainCreateInfoKHR createInfo = getCreateInfo(device, surface);

    if (vkCreateSwapchainKHR(device.getVkDevice(), &createInfo, nullptr, &m_SwapChain) != VK_SUCCESS) {
        throw std::runtime_error("ERROR: Failed to create swapchain.");
    }

    vkGetSwapchainImagesKHR(device.getVkDevice(), m_SwapChain, &m_ImageCount, nullptr);
    m_Images.resize(m_ImageCount);
    vkGetSwapchainImagesKHR(device.getVkDevice(), m_SwapChain, &m_ImageCount, m_Images.data());

    m_ImageViews.resize(m_ImageCount);
    for (int i = 0; i < m_ImageCount; i++) {
        m_ImageViews[i].init(device.getVkDevice(), m_Images[i], m_Format);
    }

    return m_SwapChain;
}

void SwapChain::deinit(const VkDevice &device, std::vector<Framebuffer> &framebuffers) {
    for (auto framebuffer : framebuffers) {
        framebuffer.deinit(device);
    }

    for (auto imageView : m_ImageViews) {
        imageView.deinit(device);
    }

    vkDestroySwapchainKHR(device, m_SwapChain, nullptr);
}

VkSwapchainCreateInfoKHR SwapChain::getCreateInfo(Device &device, const VkSurfaceKHR &surface) {
    VkPhysicalDevice physicalDevice = device.getVkPhysicalDevice();
    SupportDetails swapChainSupport = querySupport(physicalDevice, surface);

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;
    createInfo.minImageCount = m_ImageCount;
    createInfo.imageFormat = m_SurfaceFormat.format;
    createInfo.imageColorSpace = m_SurfaceFormat.colorSpace;
    createInfo.imageExtent = m_Extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    Device::QueueFamilyIndices indices = device.findQueueFamilies(physicalDevice, surface);
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = m_PresentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    return createInfo;
}

SwapChain::SupportDetails SwapChain::querySupport(const VkPhysicalDevice &physicalDevice, const VkSurfaceKHR &surface) {
    SupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

VkSurfaceFormatKHR SwapChain::chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats) {
    for (const auto &availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR SwapChain::choosePresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes) {
    for (const auto &availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D SwapChain::chooseExtent(const VkSurfaceCapabilitiesKHR &capabilities, GLFWwindow *window) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    } else {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)};

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

} // namespace Vulkan