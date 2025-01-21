#include "Device.h"

#include <iostream>

namespace VKAPP {

void Device::init(const VkInstance &instance) {
    pickPhysicalDevice(instance);
    createLogicalDevice();
}

Device::~Device() {
    vkDestroyDevice(m_Device, nullptr);
}

void Device::pickPhysicalDevice(const VkInstance &instance) {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    if (deviceCount == 0) {
        throw std::runtime_error("ERROR: Vulkan not supported on any valid device on this system.");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    int bestDeviceScore = -1;
    VkPhysicalDeviceProperties properties, bestProperties;
    for (const auto &device : devices) {
        int score = getScore(device, properties);
        if (score > bestDeviceScore) {
            m_PhysicalDevice = device;
            bestDeviceScore = score;
            bestProperties = properties;
        }
    }

    if (m_PhysicalDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("ERROR: Failed to find a GPU with Vulkan support.");
    }

    std::clog << "USING " << deviceString(bestProperties.deviceType) << ": [" << bestProperties.deviceName << "]" << std::endl;
}

void Device::createLogicalDevice() {
    QueueFamilyIndices indices = findQueueFamilies(m_PhysicalDevice);

    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
    queueCreateInfo.queueCount = 1;

    float queuePriority = 1.0f;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkPhysicalDeviceFeatures deviceFeatures{};
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.queueCreateInfoCount = 1;

    createInfo.pEnabledFeatures = &deviceFeatures;

    if (vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }
}

QueueFamilyIndices Device::findQueueFamilies(const VkPhysicalDevice &device) {
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto &queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }

        VkBool32 presentSupport = false;
        // vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

        if (presentSupport) {
            indices.presentFamily = i;
        }

        if (indices.isComplete()) {
            break;
        }

        i++;
    }

    return indices;
}

const char *Device::deviceString(const VkPhysicalDeviceType &type) {
    switch (type) {
    case VK_PHYSICAL_DEVICE_TYPE_CPU:
        return "CPU";
    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
        return "INTEGRATED GPU";
    case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
        return "VIRTUAL GPU";
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
        return "DISCRETE GPU";
    default:
        return "UNKNOWN";
    }
}

int Device::getScore(const VkPhysicalDevice &device, VkPhysicalDeviceProperties &properties) {
    QueueFamilyIndices indices = findQueueFamilies(device);
    if (!indices.isComplete()) {
        return -1;
    }
    /*if (!checkDeviceExtensionSupport(device))
        return -1;*/

    /*
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
    bool validSwapChain = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    if (!validSwapChain)
        return -1;
    */

    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceProperties(device, &properties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    // List of valid device types, higher index is better
    const VkPhysicalDeviceType deviceTypes[4] = {
        VK_PHYSICAL_DEVICE_TYPE_CPU,
        VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU,
        VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU,
        VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU};

    for (int i = 0; i < 4; i++) {
        if (properties.deviceType == deviceTypes[i]) {
            return i;
        }
    }

    return -1;
}

} // namespace VKAPP