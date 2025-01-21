#include "Application.h"
#include <iostream>

namespace VKAPP {

Application::Application() {
    m_Window.init();
    m_Instance.init();
    m_Surface.init(m_Instance.getVkInstance(), m_Window.getVkWindow());
    m_Device.init(m_Instance.getVkInstance(), m_Surface.getVkSurface());
}

Application::~Application() {
    m_Device.deinit();
    m_Surface.deinit(m_Instance.getVkInstance());
    m_Instance.deinit();
    m_Window.deinit();
}

void Application::run() {
    while (!glfwWindowShouldClose(m_Window.getVkWindow())) {
        glfwPollEvents();
    }
}

} // namespace VKAPP