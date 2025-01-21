#pragma once

#include "VulkanApp.h"

#include <vector>

namespace Vulkan {

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

} // namespace Vulkan