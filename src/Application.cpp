#include "Application.h"

#include <iostream>

namespace Vulkan {

Application::~Application() {}

void Application::run() {
    std::cout << "RUNNING\n";
}

}  // namespace Vulkan