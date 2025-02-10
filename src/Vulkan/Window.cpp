#include "Window.h"

#include <stdexcept>

namespace Vulkan {

GLFWwindow* Window::init() {
    if (!glfwInit()) {
        throw std::runtime_error("ERROR: Failed to initialize GLFW.");
    }

    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* vidmode = glfwGetVideoMode(primaryMonitor);

    const float scale = 0.8;
    const int windowWidth  = static_cast<int>(vidmode->width * scale);
    const int windowHeight = static_cast<int>(vidmode->height * scale);

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    m_Window = glfwCreateWindow(windowWidth, windowHeight, "vk-pathtracer", nullptr, nullptr);
    if (!m_Window) {
        glfwTerminate();
        throw std::runtime_error("ERROR: Failed to create GLFW window.");
    }

    glfwSetWindowUserPointer(m_Window, this);
    glfwSetFramebufferSizeCallback(m_Window, onResize);
    glfwSetWindowSizeLimits(m_Window, 200, 100, GLFW_DONT_CARE, GLFW_DONT_CARE);

    return m_Window;
}
    

void Window::onResize(GLFWwindow *window, int width, int height) {
    auto appWindow = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
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