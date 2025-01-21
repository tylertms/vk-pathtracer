#include "Device.h"

#include <iostream>
#include <set>

namespace VKAPP {

void Device::init(const VkInstance &instance, const VkSurfaceKHR &surface) {
    ext_Instance = instance;
    ext_Surface = surface;

    pickPhysicalDevice();
    createLogicalDevice();
}

void Device::deinit() {
    vkDestroyDevice(m_Device, nullptr);
}

void Device::pickPhysicalDevice() {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(ext_Instance, &deviceCount, nullptr);

    if (deviceCount == 0) {
        throw std::runtime_error("ERROR: Vulkan not supported on any valid device on this system.");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(ext_Instance, &deviceCount, devices.data());

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

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if (vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }

    vkGetDeviceQueue(m_Device, indices.graphicsFamily.value(), 0, &graphicsQueue);
    vkGetDeviceQueue(m_Device, indices.presentFamily.value(), 0, &presentQueue);
}

QueueFamilyIndices Device::findQueueFamilies(const VkPhysicalDevice &physicalDevice) {
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto &queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, ext_Surface, &presentSupport);

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

int Device::getScore(const VkPhysicalDevice &physicalDevice, VkPhysicalDeviceProperties &properties) {
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
    if (!indices.isComplete() || !deviceSupportsExtensions(physicalDevice)) {
        return -1;
    }

    /*
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
    bool validSwapChain = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    if (!validSwapChain)
        return -1;
    */

    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceProperties(physicalDevice, &properties);
    vkGetPhysicalDeviceFeatures(physicalDevice, &features);

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

bool Device::deviceSupportsExtensions(const VkPhysicalDevice &physicalDevice) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto &extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

} // namespace VKAPP