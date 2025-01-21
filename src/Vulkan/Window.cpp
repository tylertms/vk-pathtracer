#include "Window.h"

#include <stdexcept>

namespace Vulkan {

GLFWwindow *Window::init() {
    if (glfwInit() != GLFW_TRUE) {
        throw std::runtime_error("ERROR: Failed to initialize GLFW.");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_Window = glfwCreateWindow(
        m_Width, m_Height, "vulkan-pathtracer", nullptr, nullptr);

    if (m_Window == NULL) {
        throw std::runtime_error("ERROR: Failed to create GLFW window.");
    }

    return m_Window;
}

void Window::deinit() {
    glfwDestroyWindow(m_Window);
    glfwTerminate();
}

} // namespace Vulkan