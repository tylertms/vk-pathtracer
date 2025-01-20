#include "Application.h"
#include <iostream>

namespace VkApp {

Application::~Application() {
}

void Application::run() {
    while (!glfwWindowShouldClose(m_AppWindow.getWindow())) {
        glfwPollEvents();
    }
}

} // namespace VkApp