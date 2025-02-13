#include "Texture.h"

#include "Buffer.h"

#include "vulkan/vulkan_core.h"
#include "../External/tiny_gltf/stb_image.h"

#include <stdexcept>
#include <vulkan/vulkan.h>

namespace Vulkan {

void createTextureImage(const std::string filename, ImageView &textureImage, const Device &device, const CommandPool &commandPool) {
    int texWidth = 1, texHeight = 1, texChannels = 4;
    VkFormat format = textureImage.getVkFormat();
    VkDeviceSize imageSize = 0;
    void *pixels = nullptr;

    printf("%d\n", format);

    if (!filename.empty()) {
        if (format == VK_FORMAT_R32G32B32A32_SFLOAT) {
            float *dataFloat = stbi_loadf(filename.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
            if (!dataFloat) {
                throw std::runtime_error("ERROR: Failed to load texture image \"" + filename + "\"!");
            }
            imageSize = static_cast<VkDeviceSize>(texWidth) * texHeight * 4 * sizeof(float);
            pixels = static_cast<void*>(dataFloat);
        } else if (format == VK_FORMAT_R8G8B8A8_UNORM) {
            unsigned char *data8 = stbi_load(filename.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
            if (!data8) {
                throw std::runtime_error("ERROR: Failed to load texture image \"" + filename + "\"!");
            }
            imageSize = static_cast<VkDeviceSize>(texWidth) * texHeight * 4 * sizeof(unsigned char);
            pixels = static_cast<void*>(data8);
        } else {
            printf("SKIPPING %s\n", filename.c_str());
            return;
        }
    } else {
        imageSize = 4 * sizeof(uint8_t);
    }

    VkExtent2D extent = {
        .width = static_cast<uint32_t>(texWidth),
        .height = static_cast<uint32_t>(texHeight)
    };

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(device, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                    stagingBuffer, stagingBufferMemory);

    void *data;
    vkMapMemory(device.getVkDevice(), stagingBufferMemory, 0, imageSize, 0, &data);
    if (pixels) {
        memcpy(data, pixels, static_cast<size_t>(imageSize));
        stbi_image_free(pixels);
    } else {
        memset(data, 0, static_cast<size_t>(imageSize));
    }
    vkUnmapMemory(device.getVkDevice(), stagingBufferMemory);

    textureImage.createImage(device, extent, format, VK_IMAGE_TILING_OPTIMAL,
                                VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    textureImage.init(device.getVkDevice(), nullptr, format);
    textureImage.createSampler(device);

    textureImage.transitionImageLayout(device, commandPool, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    commandPool.copyBufferToImage(device, stagingBuffer, textureImage.getVkImage(), extent);
    textureImage.transitionImageLayout(device, commandPool, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vkDestroyBuffer(device.getVkDevice(), stagingBuffer, nullptr);
    vkFreeMemory(device.getVkDevice(), stagingBufferMemory, nullptr);
}

}
