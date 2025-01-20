#pragma once

#include "Vulkan.hpp"

#include <vector>

namespace Vulkan {

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

} // namespace Vulkan