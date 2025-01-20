#include "Application.h"
#include <iostream>

namespace Vulkan {

Application::~Application() {
}

void Application::run() {
    while (!glfwWindowShouldClose(m_AppWindow.getWindow())) {
        glfwPollEvents();
    }
}

} // namespace Vulkan