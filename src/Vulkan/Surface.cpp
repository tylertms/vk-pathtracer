#include "Surface.h"

#include <stdexcept>

namespace Vulkan {

VkSurfaceKHR Surface::init(const VkInstance &instance, GLFWwindow *window) {
    ext_Instance = instance;

    if (glfwCreateWindowSurface(instance, window, nullptr, &m_Surface) != VK_SUCCESS) {
        throw std::runtime_error("ERROR: Failed to create window surface.");
    }

    return m_Surface;
}

void Surface::deinit() {
    vkDestroySurfaceKHR(ext_Instance, m_Surface, nullptr);
}

} // namespace Vulkan