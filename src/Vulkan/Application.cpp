#include "Application.h"
#include <iostream>

namespace Vulkan {

Application::Application() {
    m_Window.init();
    m_Instance.init();
    m_Surface.init(m_Instance.getVkInstance(), m_Window.getGlfwWindow());
    m_Device.init(m_Instance.getVkInstance(), m_Surface.getVkSurface());
    m_SwapChain.init(m_Device, m_Surface.getVkSurface(), m_Window.getGlfwWindow());
}

Application::~Application() {
    m_SwapChain.deinit(m_Device.getVkDevice());
    m_Device.deinit();
    m_Surface.deinit();
    m_Instance.deinit();
    m_Window.deinit();
}

void Application::run() {
    while (!glfwWindowShouldClose(m_Window.getGlfwWindow())) {
        glfwPollEvents();
    }
}

} // namespace Vulkan