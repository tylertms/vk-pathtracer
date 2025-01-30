#include "UserInterface.h"

#include "../IO/FilePicker.h"
#include "../IO/SceneLoader.h"

namespace Interface {

void UserInterface::init(const Vulkan::Device &device, const Vulkan::Instance &instance,
                         const Vulkan::Window &window, const Vulkan::DescriptorPool &descriptorPool,
                         const Vulkan::SwapChain &swapChain,
                         const Vulkan::GraphicsPipeline &graphicsPipeline) {

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.Fonts->AddFontFromFileTTF("assets/fonts/Lato.ttf", 16);

    ImGui::StyleColorsDark();
    ImGuiStyle &style = ImGui::GetStyle();
    style.WindowRounding = 2;
    style.GrabRounding = 2;
    style.FrameRounding = 2;
    style.FrameBorderSize = 0;
    style.WindowBorderSize = 0;
    style.DockingSeparatorSize = 1;
    style.WindowPadding = { 8, 5 };

    style.Colors[ImGuiCol_WindowBg] = ImVec4(0, 0, 0, 0);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.14, 0.14, 0.14, 1.0);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.1, 0.1, 0.1, 1.0);

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
    m_FrameTimes.resize(100, 0);
}

void UserInterface::deinit() {
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void UserInterface::drawStats(Vulkan::SceneManager &sceneManager) {

    ImGui::Begin("Statistics");
    ImGui::Text("Frames: %d", sceneManager.sceneData.framesRendered);

    float total = 0;
    uint32_t count = 0;
    while (total < 1000 && count < m_FrameTimes.size() - 1) {
        total += m_FrameTimes[m_FrameTimes.size() - count - 1];
        count++;
    }
    float avg = total / count;

    ImGui::Text("Average: %6.2fms", avg);

    ImGui::PlotLines("##", m_FrameTimes.data(), m_FrameTimes.size(), 0, __null, 0.0, avg * 2, ImVec2(160.0f, 40.0f));

    if (ImGui::Button("Reset Accumulation"))
        sceneManager.resetAccumulation();

    ImGui::End();
}

void UserInterface::drawMenuBar(Vulkan::SceneManager &sceneManager) {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Add")) {
            if (ImGui::MenuItem("Sphere")) {
                sceneManager.addSphere();
                sceneManager.resetAccumulation();
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Load")) {
            if (ImGui::MenuItem("Scene")) {
                Loader::loadSceneFromYAML(pickFilePath(VKPT_SCENE, VKPT_LOAD), sceneManager);
            }
            if (ImGui::MenuItem("Model")) {
                sceneManager.addMesh(pickFilePath(VKPT_MODEL, VKPT_LOAD));
                sceneManager.updateMeshTransforms();
                sceneManager.uploadFullStorageBuffer();
                sceneManager.resetAccumulation();
            }
            ImGui::EndMenu();
        }

        if (ImGui::MenuItem("Save")) {
            Loader::saveSceneToYAML(pickFilePath(VKPT_SCENE, VKPT_SAVE), sceneManager);
        }

        if (ImGui::BeginMenu("View")) {
            ImGui::Checkbox("Show Camera Control", &m_ShowCameraControl);
            ImGui::Checkbox("Show Object Control", &m_ShowObjectControl);
            ImGui::Checkbox("Show Statistics", &m_ShowStats);
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void UserInterface::setupDockspace() {
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);

    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

    ImGui::Begin("DockSpace", nullptr, window_flags);

    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(2);

    ImGuiID dockspace_id = ImGui::GetID("Dockspace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
    ImGui::End();
}

void UserInterface::draw(Vulkan::SceneManager &sceneManager, ImVec2 &position, ImVec2 &extent) {
    m_TimeCurrent = std::chrono::high_resolution_clock::now();
    float frameTime = std::chrono::duration<float, std::milli>(m_TimeCurrent - m_TimeStart).count();

    m_FrameTimes.push_back(frameTime);
    if (m_FrameTimes.size() > 100)
        m_FrameTimes.erase(m_FrameTimes.begin());

    m_TimeStart = m_TimeCurrent;

    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    setupDockspace();
    drawMenuBar(sceneManager);

    if (m_ShowCameraControl) {
        if (drawCameraControl(sceneManager)) {
            sceneManager.resetAccumulation();
        }
    }

    if (m_ShowObjectControl) {
        drawObjectControl(sceneManager);
    }

    if (m_ShowStats) {
        drawStats(sceneManager);
    }

    ImGui::Begin("Viewport");
    position = ImGui::GetWindowPos();
    extent = ImGui::GetWindowSize();
    ImGui::End();

    ImGui::Render();
}

bool UserInterface::drawCameraControl(Vulkan::SceneManager &sceneManager) {
    ImGui::Begin("Camera");

    auto &camera = sceneManager.sceneData.camera;
    bool reset = false;

    uint32_t samplesMin = 1, samplesMax = 100;
    uint32_t bouncesMin = 0, bouncesMax = 30;

    if (ImGui::DragScalar("Samples Per Pixel", ImGuiDataType_U32, &camera.samplesPerPixel, 1, &samplesMin, &samplesMax)) {}
    if (ImGui::DragScalar("Max Bounces", ImGuiDataType_U32, &camera.maxBounces, 1, &bouncesMin, &bouncesMax)) reset = true;
    if (ImGui::DragFloat3("Look From", (float *)&camera.lookFrom)) reset = true;
    if (ImGui::DragFloat3("Look At", (float *)&camera.lookAt)) reset = true;
    if (ImGui::DragFloat("VFOV", &camera.vfov, 0.f, 180.f)) reset = true;
    if (ImGui::DragFloat("Focal Distance", &camera.focalDistance, 0.01f, 0.1f, 100.f)) reset = true;
    if (ImGui::DragFloat("Defocus Strength", &camera.defocus, 0.1f, 0.f, 1000.f)) reset = true;
    if (ImGui::DragFloat("Diverge Strength", &camera.diverge, 0.1f, 0.f, 1000.f)) reset = true;

    ImGui::End();
    return reset;
}

bool UserInterface::drawObjectControl(Vulkan::SceneManager &sceneManager) {
    ImGui::Begin("Objects");

    for (uint32_t i = 0; i < sceneManager.sceneData.numSpheres; i++) {
        ImGui::PushID("##sphere");
        ImGui::PushID(i);
        if (ImGui::CollapsingHeader("Sphere")) {
            if (drawSphereControl(sceneManager.sceneStorage->spheres[i])) {
                sceneManager.uploadPartialStorageBuffer();
                sceneManager.resetAccumulation();
            }
        }
        ImGui::PopID();
        ImGui::PopID();
    }

    for (uint32_t i = 0; i < sceneManager.sceneData.numMeshes; i++) {
        ImGui::PushID("##mesh");
        ImGui::PushID(i);
        if (ImGui::CollapsingHeader("Mesh")) {
            if (drawMeshControl(sceneManager, i)) {
                sceneManager.updateMeshTransforms();
                sceneManager.uploadPartialStorageBuffer();
                sceneManager.resetAccumulation();
            }
        }
        ImGui::PopID();
        ImGui::PopID();
    }

    ImGui::End();
    return true;
}

bool UserInterface::drawSphereControl(VKPT::Sphere &sphere) {
    bool reset = false;

    if (ImGui::DragFloat3("Position", (float *)(&sphere.center), 0.01)) reset = true;
    if (ImGui::DragFloat("Radius", &sphere.radius, 0.01)) reset = true;
    if (ImGui::ColorEdit3("Color", (float *)(&sphere.material.color))) reset = true;
    if (ImGui::DragFloat("Roughness", &sphere.material.roughness, 0.01, 0.0, 1.0)) reset = true;
    if (ImGui::ColorEdit3("Specular Color", (float *)(&sphere.material.specularColor))) reset = true;
    if (ImGui::DragFloat("Specular Factor", &sphere.material.specularFactor, 0.01, 0.0, 1.0)) reset = true;
    if (ImGui::ColorEdit3("Emission Color", (float *)(&sphere.material.emissionColor))) reset = true;
    if (ImGui::DragFloat("Emission Strength", &sphere.material.emissionStrength, 0.01)) reset = true;
    ImGui::Spacing();

    return reset;
}

bool UserInterface::drawMeshControl(Vulkan::SceneManager &sceneManager, uint32_t index) {
    bool reset = false;
    VKPT::Mesh &mesh = sceneManager.sceneStorage->meshes[index];

    if (ImGui::DragFloat3("Translation", (float *)(&sceneManager.meshTransforms[index][0]), 0.01)) reset = true;
    if (ImGui::DragFloat3("Rotation", (float *)(&sceneManager.meshTransforms[index][1]), 0.5)) reset = true;
    if (ImGui::DragFloat3("Scale", (float *)(&sceneManager.meshTransforms[index][2]), 0.01)) reset = true;
    if (ImGui::ColorEdit3("Color", (float *)(&mesh.material.color))) reset = true;
    if (ImGui::DragFloat("Roughness", &mesh.material.roughness, 0.01, 0.0, 1.0)) reset = true;
    if (ImGui::ColorEdit3("Specular Color", (float *)(&mesh.material.specularColor))) reset = true;
    if (ImGui::DragFloat("Specular Factor", &mesh.material.specularFactor, 0.01, 0.0, 1.0)) reset = true;
    if (ImGui::ColorEdit3("Emission Color", (float *)(&mesh.material.emissionColor))) reset = true;
    if (ImGui::DragFloat("Emission Strength", &mesh.material.emissionStrength, 0.01)) reset = true;
    ImGui::Spacing();

    return reset;
}

} // namespace Interface
