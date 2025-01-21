#include "Surface.h"

#include <stdexcept>

namespace Vulkan {

void Surface::init(const VkInstance &instance, GLFWwindow *window) {
    if (glfwCreateWindowSurface(instance, window, nullptr, &m_Surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
}

void Surface::deinit(const VkInstance &instance) {
    vkDestroySurfaceKHR(instance, m_Surface, nullptr);
}


} // namespace Vulkan