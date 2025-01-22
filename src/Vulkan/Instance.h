#pragma once

#include "VulkanApp.h"

#include <vector>

namespace Vulkan {

class Instance {
  public:
    NO_COPY(Instance);
    Instance() = default;

    VkInstance init();
    void deinit();

    inline const VkInstance &getVkInstance() const { return m_Instance; }

  private:
    VkInstance m_Instance;
    VkDebugUtilsMessengerEXT m_DebugMessenger;
    std::vector<const char *> m_Extensions;

    VkInstanceCreateInfo getCreateInfo();
    VkApplicationInfo getAppInfo();
    void attachExtensions(VkInstanceCreateInfo &createInfo);
};

} // namespace Vulkan