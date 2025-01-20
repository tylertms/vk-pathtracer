#pragma once

#include "VkApp.h"

#include <vector>

namespace VKAPP {

class Instance {
  public:
    Instance();
    ~Instance();

  private:
    VkInstance m_Instance;
    std::vector<const char *> m_Extensions;

    VkInstanceCreateInfo getCreateInfo();
    VkApplicationInfo getAppInfo();
    void attachExtensions(VkInstanceCreateInfo &createInfo);
};

} // namespace VKAPP