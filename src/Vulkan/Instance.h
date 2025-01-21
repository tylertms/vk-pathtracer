#pragma once

#include "VkApp.h"

#include <vector>

namespace VKAPP {

class Instance {
  public:
    void init();
    ~Instance();

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