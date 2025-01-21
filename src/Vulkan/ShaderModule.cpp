#include "ShaderModule.h"

#include <fstream>

namespace Vulkan {

ShaderModule::ShaderModule(const VkDevice &device, const std::string &filename, VkShaderStageFlagBits stageFlag) {
    std::vector<char> code = readFile(filename);
    m_StageFlag = stageFlag;

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    if (vkCreateShaderModule(device, &createInfo, nullptr, &m_ShaderModule) != VK_SUCCESS) {
        throw std::runtime_error("ERROR: Failed to create shader module.");
    }
}

void ShaderModule::deinit(const VkDevice &device) {
    vkDestroyShaderModule(device, m_ShaderModule, nullptr);
}

VkPipelineShaderStageCreateInfo ShaderModule::getPipelineStageCreateInfo() {
    VkPipelineShaderStageCreateInfo shaderStageInfo{};
    shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageInfo.stage = m_StageFlag;
    shaderStageInfo.module = m_ShaderModule;
    shaderStageInfo.pName = "main";

    return shaderStageInfo;
}

std::vector<char> ShaderModule::readFile(const std::string &filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        std::string error = "ERROR: Failed to open file: " + filename;
        throw std::runtime_error(error);
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}

} // namespace Vulkan