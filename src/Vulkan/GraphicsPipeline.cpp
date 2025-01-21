#include "GraphicsPipeline.h"

#include "ShaderModule.h"

namespace Vulkan {

void GraphicsPipeline::init(const VkDevice &device) {
    ShaderModule vertShader(device, "build/shaders/main.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
    ShaderModule fragShader(device, "build/shaders/main.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);

    VkPipelineShaderStageCreateInfo shaderStages[] = {
        vertShader.getPipelineStageCreateInfo(), 
        fragShader.getPipelineStageCreateInfo()
    };
    
    vertShader.deinit(device);
    fragShader.deinit(device);
}

void GraphicsPipeline::deinit() {

}

} // namespace Vulkan
