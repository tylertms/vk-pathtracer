#pragma once

#include "VulkanApp.h"
#include <vector>

namespace Vulkan {

extern const std::vector<const char *> g_ValidationLayers;
extern const bool g_EnabledValidationLayers;

bool validationLayersSupported();
void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

VkResult createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger);
void destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator);
void setupDebugMessenger(const VkInstance &instance, VkDebugUtilsMessengerEXT &debugMessenger);

} // namespace Vulkan
