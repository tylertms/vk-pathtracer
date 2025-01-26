#include "UserInterface.h"

#include "../../External/ImGui/backends/imgui_impl_glfw.h"
#include "../../External/ImGui/backends/imgui_impl_vulkan.h"
#include "../../External/ImGui/imgui.h"
#include "../Loader/GLTFLoader.h"

#include <cmath>

namespace Interface {

void UserInterface::init(const Vulkan::Device &device, const Vulkan::Instance &instance,
                         const Vulkan::Window &window, const Vulkan::DescriptorPool &descriptorPool,
                         const Vulkan::SwapChain &swapChain,
                         const Vulkan::GraphicsPipeline &graphicsPipeline) {

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
    info.QueueFamily = device.findQueueFamilies(device.getVkPhysicalDevice(), nullptr).graphicsFamily.value();
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

void UserInterface::deinit() {
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void UserInterface::draw(Vulkan::Scene &scene) {
    VKPT::SceneObject *sceneObj = scene.getObject();

    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Text("Frames: %d", sceneObj->framesRendered);
    ImGui::Text("FPS: %d", m_DisplayFPS);
    if (ImGui::Button("Reset Accumulation"))
        scene.resetAccumulation();
    ImGui::Spacing();

    if (ImGui::Button("Add Sphere"))
        scene.addSphere();
    if (ImGui::Button("Load Scene")) {
        scene.triangleBuffer.clear();
        scene.loadSceneFromFile("assets/scenes/monkey.yaml");
    }
    if (ImGui::Button("Load Mesh")) {
        Loader::GLTFLoader m("assets/models/suzanne.glb");
        scene.addMesh(m.getTriangles());
    }

    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Objects", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Indent();
        for (uint32_t i = 0; i < sceneObj->numSpheres; i++) {
            ImGui::PushID(i);
            ImGui::Text("Sphere %d", i + 1);
            if (drawSphereControl(sceneObj->spheres[i]))
                scene.resetAccumulation();

            ImGui::PopID();
        }

        for (uint32_t i = 0; i < sceneObj->numMeshes; i++) {
            ImGui::PushID(sceneObj->numSpheres + i);
            ImGui::Text("Mesh %d - %d triangles", i + 1, sceneObj->meshes[i].triangleCount);
            if (drawMeshControl(sceneObj->meshes[i]))
                scene.applyMeshProperties(sceneObj->meshes[i]);
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

bool UserInterface::drawSphereControl(VKPT::Sphere &sphere) {
    bool reset = false;

    if (ImGui::DragFloat3("Position", (float*)(&sphere.center), 0.01)) reset = true;
    if (ImGui::DragFloat("Radius", &sphere.radius, 0.01)) reset = true;
    if (ImGui::ColorEdit3("Color", (float*)(&sphere.material.color))) reset = true;
    if (ImGui::ColorEdit3("Emission Color", (float*)(&sphere.material.emissionColor))) reset = true;
    if (ImGui::DragFloat("Emission Strength", &sphere.material.emissionStrength, 0.01)) reset = true;
    return reset;
}

bool UserInterface::drawMeshControl(VKPT::Mesh &mesh) {
    bool reset = false;
    if (ImGui::DragFloat3("Translation", (float *)(&mesh.translation), 0.01)) reset = true;
    if (ImGui::DragFloat3("Scale", (float *)(&mesh.scale), 0.01)) reset = true;
    if (ImGui::DragFloat3("Rotation", (float *)(&mesh.rotation), 0.1)) reset = true;
        if (ImGui::ColorEdit3("Color", (float *)(&mesh.material.color))) reset = true;
        if (ImGui::ColorEdit3("Emission Color", (float *)(&mesh.material.emissionColor))) reset = true;
    if (ImGui::DragFloat("Emission Strength", &mesh.material.emissionStrength, 0.01)) reset = true;
    return reset;
}

} // namespace Interface
