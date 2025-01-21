#pragma once

#include "VulkanApp.h"

#include <vector>

namespace Vulkan {

class ShaderModule {
  public:
    ShaderModule(const VkDevice &device, const std::string &filename, VkShaderStageFlagBits stageFlag);
    void deinit(const VkDevice &device);

    VkPipelineShaderStageCreateInfo getPipelineStageCreateInfo();

    const VkShaderModule &getVkShaderModule() const { return m_ShaderModule; }

  private:
    VkShaderModule m_ShaderModule;
    VkShaderStageFlagBits m_StageFlag;

    static std::vector<char> readFile(const std::string &filename);
};

} // namespace Vulkan
