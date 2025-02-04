#include "Texture.h"

#include "Buffer.h"
#include "vulkan/vulkan_core.h"
#include "../External/tiny_gltf/stb_image.h"

#include <stdexcept>
#include <vulkan/vulkan.h>

namespace Vulkan {

void createTextureImage(ImageView &textureImage, const Device &device, const CommandPool &commandPool) {
    int texWidth, texHeight, texChannels;
    float* pixels = stbi_loadf("assets/hdri/spruit_sunrise_4k.hdr", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * texHeight * 4 * sizeof(float);

    if (!pixels) {
        throw std::runtime_error("ERROR: Failed to load texture image!");
    }

    VkExtent2D extent = {
        .width = static_cast<uint32_t>(texWidth),
        .height = static_cast<uint32_t>(texHeight)
    };

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(device, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void *data;
    vkMapMemory(device.getVkDevice(), stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, pixels, static_cast<size_t>(imageSize));
    vkUnmapMemory(device.getVkDevice(), stagingBufferMemory);

    stbi_image_free(pixels);

    textureImage.createImage(device, extent, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    textureImage.transitionImageLayout(device, commandPool, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    commandPool.copyBufferToImage(device, stagingBuffer, textureImage.getVkImage(), extent);

    textureImage.transitionImageLayout(device, commandPool, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    textureImage.init(device.getVkDevice(), nullptr, VK_FORMAT_R32G32B32A32_SFLOAT);
    textureImage.createSampler(device);

    vkDestroyBuffer(device.getVkDevice(), stagingBuffer, nullptr);
    vkFreeMemory(device.getVkDevice(), stagingBufferMemory, nullptr);
}

}
