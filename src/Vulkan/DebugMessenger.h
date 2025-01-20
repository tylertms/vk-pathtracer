#pragma once

#include "VkApp.h"
#include <vector>
#include <vulkan/vulkan.h>

namespace VKAPP {

extern const std::vector<const char *> g_ValidationLayers;
extern const bool g_EnabledValidationLayers;

bool validationLayersSupported();
void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

VkResult createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger);
void destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator);
void setupDebugMessenger(const VkInstance &instance, VkDebugUtilsMessengerEXT &debugMessenger);

} // namespace VKAPP
