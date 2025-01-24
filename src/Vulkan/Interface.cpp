#include "Interface.h"
#include "DescriptorPool.h"
#include "GraphicsPipeline.h"
#include "SwapChain.h"

#include "../External/ImGui/backends/imgui_impl_glfw.h"
#include "../External/ImGui/backends/imgui_impl_vulkan.h"
#include "../External/ImGui/imgui.h"
#include "Vulkan/Scene.h"

#include <cmath>

namespace Vulkan {

void Interface::init(const Device &device, const Instance &instance,
                     const Window &window, const DescriptorPool &descriptorPool,
                     const SwapChain &swapChain,
                     const GraphicsPipeline &graphicsPipeline) {

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForVulkan(window.getGlfwWindow(), true);
    ImGui_ImplVulkan_InitInfo info{};
    info.Instance = instance.getVkInstance();
    info.PhysicalDevice = device.getVkPhysicalDevice();
    info.Device = device.getVkDevice();
    info.Queue = device.getGraphicsQueue();
    info.QueueFamily =
        device.findQueueFamilies(device.getVkPhysicalDevice(), nullptr)
            .graphicsFamily.value();
    info.PipelineCache = VK_NULL_HANDLE;
    info.DescriptorPool = descriptorPool.getVkDescriptorPool();
    info.Allocator = VK_NULL_HANDLE;
    info.MinImageCount = static_cast<uint32_t>(swapChain.getImageCount() - 1);
    info.ImageCount = static_cast<uint32_t>(swapChain.getImageCount());
    info.CheckVkResultFn = nullptr;
    info.RenderPass = graphicsPipeline.getVkRenderPass();

    ImGui_ImplVulkan_Init(&info);
    ImGui_ImplVulkan_CreateFontsTexture();

    m_TimeStart = std::chrono::high_resolution_clock::now();
}

void Interface::deinit() {
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Interface::draw(Scene &scene) {
    SceneObject *sceneObj = scene.getObject();

    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Text("Frames: %d", sceneObj->framesRendered);
    ImGui::Text("FPS: %d", m_DisplayFPS);
    if (ImGui::Button("Reset Accumulation")) scene.resetAccumulation();
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Objects")) {
        ImGui::Indent();
        for (int i = 0; i < sceneObj->numSpheres; i++) {
            ImGui::PushID(i);
            ImGui::Text("Sphere %d", i + 1);
            if (drawSphereControl(sceneObj->spheres[i]))
                scene.resetAccumulation();
            ImGui::PopID();
        }
        ImGui::Unindent();
    }

    ImGui::Render();

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }

    m_TimeCurrent = std::chrono::high_resolution_clock::now();
    float timeSinceStart = std::chrono::duration<float, std::milli>(m_TimeCurrent - m_TimeStart).count();
    m_FramesLastSecond++;

    if (timeSinceStart > 1000) {
        m_DisplayFPS = floorf(1000.0 * m_FramesLastSecond / timeSinceStart);
        m_FramesLastSecond = 0;
        m_TimeStart = m_TimeCurrent;
    }
}

bool Interface::drawSphereControl(Sphere &sphere) {
    bool reset = false;
    if(ImGui::DragFloat3("Position", sphere.center, 0.01)) reset = true;
    if(ImGui::DragFloat("Radius", &sphere.radius, 0.01)) reset = true;
    if(ImGui::ColorEdit3("Color", sphere.material.color)) reset = true;
    if(ImGui::ColorEdit3("Emission Color", sphere.material.emissionColor)) reset = true;
    if(ImGui::DragFloat("Emission Strength", &sphere.material.emissionStrength, 0.01)) reset = true;
    return reset;
}

} // namespace Vulkan
