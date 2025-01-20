#include "Application.h"
#include <iostream>

namespace VKAPP {

Application::~Application() {
}

void Application::run() {
    while (!glfwWindowShouldClose(m_AppWindow.getWindow())) {
        glfwPollEvents();
    }
}

} // namespace VKAPP