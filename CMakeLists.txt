cmake_minimum_required(VERSION 3.20...3.31)
project(vulkan-pathtracer VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

add_compile_options(-Wall)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(WIN32)
    add_definitions(-DPLATFORM_WINDOWS)
elseif(APPLE)
    add_definitions(-DPLATFORM_MAC)
elseif(UNIX)
    add_definitions(-DPLATFORM_LINUX)
endif()

if(CMAKE_BUILD_TYPE MATCHES DEBUG)
    add_compile_options(-g -O0 -fno-omit-frame-pointer)
    if(!WIN32)
        add_compile_options(-fsanitize=address)
        add_link_options(-fsanitize=address)
    endif()
else()
    add_compile_options(-O3 -march=native)
    add_compile_definitions(PRIVATE NODEBUG=1)
endif()

find_package(Vulkan REQUIRED)
find_program(GLSLC glslc REQUIRED)

include(FetchContent)
FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG 3.4)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_Declare(glm GIT_REPOSITORY https://github.com/g-truc/glm.git GIT_TAG 1.0.1)
FetchContent_Declare(yaml-cpp GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git GIT_TAG 0.8.0)
FetchContent_Declare(nfd GIT_REPOSITORY https://github.com/btzy/nativefiledialog-extended.git GIT_TAG v1.2.1)
FetchContent_MakeAvailable(glfw glm yaml-cpp nfd)

file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp" "${CMAKE_SOURCE_DIR}/src/*.hpp")
add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src ${Vulkan_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan glfw glm yaml-cpp::yaml-cpp nfd)

set(SHADER_OUTPUT_DIR "${CMAKE_BINARY_DIR}/shaders_spv")
file(MAKE_DIRECTORY ${SHADER_OUTPUT_DIR})

set(SHADER_SRCS "Graphics.vert" "Graphics.frag")
file(GLOB_RECURSE SHADER_DEPS "${CMAKE_SOURCE_DIR}/src/Shaders/*.glsl" "${CMAKE_SOURCE_DIR}/src/Core/*.h")

foreach(SHADER ${SHADER_SRCS})
    add_custom_command(
        OUTPUT "${SHADER_OUTPUT_DIR}/${SHADER}.spv"
        COMMAND ${GLSLC} -c "${CMAKE_SOURCE_DIR}/src/Shaders/${SHADER}" -O -o "${SHADER_OUTPUT_DIR}/${SHADER}.spv"
        DEPENDS "${CMAKE_SOURCE_DIR}/src/Shaders/${SHADER}" ${SHADER_DEPS}
        COMMENT "Compiling ${SHADER} to SPIR-V"
    )
    list(APPEND SPIRV_BINARY "${SHADER_OUTPUT_DIR}/${SHADER}.spv")
endforeach()

add_custom_target(Shaders DEPENDS ${SPIRV_BINARY})
add_dependencies(${PROJECT_NAME} Shaders)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${SHADER_OUTPUT_DIR} $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders_spv
)

target_compile_definitions(${PROJECT_NAME} PRIVATE SHADER_DIR=\"shaders_spv\")
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
