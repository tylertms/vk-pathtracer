#include "Window.h"

#include <stdexcept>

namespace Vulkan {

GLFWwindow *Window::init() {
    if (glfwInit() != GLFW_TRUE) {
        throw std::runtime_error("ERROR: Failed to initialize GLFW.");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, VK_TRUE);

    m_Window = glfwCreateWindow(m_Width, m_Height, "vulkan-pathtracer", nullptr, nullptr);

    glfwSetWindowUserPointer(m_Window, this);
    glfwSetFramebufferSizeCallback(m_Window, onResize);
    glfwSetWindowSizeLimits(m_Window, 200, 100, GLFW_DONT_CARE, GLFW_DONT_CARE);

    if (m_Window == NULL) {
        throw std::runtime_error("ERROR: Failed to create GLFW window.");
    }

    return m_Window;
}

void Window::onResize(GLFWwindow *window, int width, int height) {
    auto appWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
    appWindow->m_Resized = true;
}

bool Window::wasResized() {
    return m_Resized;
}

void Window::clearResized() {
    m_Resized = false;
}

void Window::deinit() {
    glfwDestroyWindow(m_Window);
    glfwTerminate();
}

} // namespace Vulkan