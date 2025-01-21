#pragma once

#include "VkApp.h"

#include <vector>

namespace VKAPP {

#define VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME "VK_KHR_portability_subset"

class Instance {
  public:
    void init();
    void deinit();

    VkInstance &getVkInstance();

  private:
    VkInstance m_Instance;
    VkDebugUtilsMessengerEXT m_DebugMessenger;
    std::vector<const char *> m_Extensions;

    VkInstanceCreateInfo getCreateInfo();
    VkApplicationInfo getAppInfo();
    void attachExtensions(VkInstanceCreateInfo &createInfo);
};

} // namespace VKAPP