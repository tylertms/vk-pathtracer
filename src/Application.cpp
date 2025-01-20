#include "Application.h"

#include <iostream>

#include "Vulkan.h"

namespace Vulkan {

Application::~Application() {}

void Application::run() {
    while (!glfwWindowShouldClose(m_AppWindow.getWindow())) {
        glfwPollEvents();
    }
}

}  // namespace Vulkan