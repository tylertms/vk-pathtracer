#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define NO_COPY(class)                        \
    class(const class &) = delete;            \
    class(class &&) = delete;                 \
    class &operator=(const class &) = delete; \
    class &operator=(class &&) = delete;