#include "Application.h"
#include <iostream>

namespace VKAPP {

Application::Application() {
    m_AppWindow.init();
    m_Instance.init();
    m_Device.init(m_Instance.getVkInstance());
}

Application::~Application() {
}

void Application::run() {
    while (!glfwWindowShouldClose(m_AppWindow.getWindow())) {
        glfwPollEvents();
    }
}

} // namespace VKAPP