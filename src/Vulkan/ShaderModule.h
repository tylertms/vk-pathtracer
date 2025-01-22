#pragma once

#include "VulkanApp.h"

#include <vector>

namespace Vulkan {

class ShaderModule {
  public:
    NO_COPY(ShaderModule);
    ShaderModule() = default;
    
    ShaderModule(const VkDevice &device, const std::string &filename, VkShaderStageFlagBits stageFlag);
    void deinit(const VkDevice &device);

    VkPipelineShaderStageCreateInfo getPipelineStageCreateInfo();

    inline const VkShaderModule &getVkShaderModule() const { return m_ShaderModule; }

  private:
    VkShaderModule m_ShaderModule;
    VkShaderStageFlagBits m_StageFlag;

    static std::vector<char> readFile(const std::string &filename);
};

} // namespace Vulkan
