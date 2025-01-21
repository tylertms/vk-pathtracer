#include "Surface.h"

#include <stdexcept>

namespace Vulkan {

void Surface::init(const VkInstance &instance, GLFWwindow *window) {
    ext_Instance = instance;

    if (glfwCreateWindowSurface(instance, window, nullptr, &m_Surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
}

void Surface::deinit() {
    vkDestroySurfaceKHR(ext_Instance, m_Surface, nullptr);
}


} // namespace Vulkan