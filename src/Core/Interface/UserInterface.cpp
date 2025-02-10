#include "UserInterface.h"

#include "CameraControl.h"
#include "ObjectEditor.h"
#include "MaterialEditor.h"

#include "../File/FilePicker.h"
#include "../File/SceneLoader.h"
#include "../Constants.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <icons/IconsFontAwesome6.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

namespace Interface {

void SetDarkTheme() {
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    const ImVec4 almostBlack  = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
    const ImVec4 darkGray     = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    const ImVec4 midGray      = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    const ImVec4 lightGray    = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    const ImVec4 textColor    = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);

    colors[ImGuiCol_Text]                   = textColor;
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg]               = almostBlack;
    colors[ImGuiCol_ChildBg]                = darkGray;
    colors[ImGuiCol_PopupBg]                = almostBlack;
    colors[ImGuiCol_Border]                 = midGray;
    colors[ImGuiCol_FrameBg]                = midGray;
    colors[ImGuiCol_FrameBgHovered]         = lightGray;
    colors[ImGuiCol_FrameBgActive]          = lightGray;
    colors[ImGuiCol_TitleBg]                = darkGray;
    colors[ImGuiCol_TitleBgActive]          = midGray;
    colors[ImGuiCol_TitleBgCollapsed]       = almostBlack;
    colors[ImGuiCol_MenuBarBg]              = darkGray;
    colors[ImGuiCol_ScrollbarBg]            = darkGray;
    colors[ImGuiCol_ScrollbarGrab]          = midGray;
    colors[ImGuiCol_ScrollbarGrabHovered]   = lightGray;
    colors[ImGuiCol_ScrollbarGrabActive]    = lightGray;
    colors[ImGuiCol_CheckMark]              = textColor;
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
    colors[ImGuiCol_Button]                 = midGray;
    colors[ImGuiCol_ButtonHovered]          = lightGray;
    colors[ImGuiCol_ButtonActive]           = lightGray;
    colors[ImGuiCol_Header]                 = midGray;
    colors[ImGuiCol_HeaderHovered]          = lightGray;
    colors[ImGuiCol_HeaderActive]           = lightGray;
    colors[ImGuiCol_Separator]              = midGray;
    colors[ImGuiCol_SeparatorHovered]       = lightGray;
    colors[ImGuiCol_SeparatorActive]        = lightGray;
    colors[ImGuiCol_ResizeGrip]             = midGray;
    colors[ImGuiCol_ResizeGripHovered]      = lightGray;
    colors[ImGuiCol_ResizeGripActive]       = lightGray;
    colors[ImGuiCol_Tab]                    = midGray;
    colors[ImGuiCol_TabHovered]             = lightGray;
    colors[ImGuiCol_TabActive]              = lightGray;
    colors[ImGuiCol_TabUnfocused]           = darkGray;
    colors[ImGuiCol_TabUnfocusedActive]     = midGray;
    colors[ImGuiCol_TabSelectedOverline]    = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_DockingPreview]         = lightGray;
    colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]       = lightGray;
    colors[ImGuiCol_PlotHistogram]          = midGray;
    colors[ImGuiCol_PlotHistogramHovered]   = lightGray;
    colors[ImGuiCol_TextSelectedBg]         = midGray;
    colors[ImGuiCol_DragDropTarget]         = lightGray;
    colors[ImGuiCol_NavHighlight]           = lightGray;
    colors[ImGuiCol_NavWindowingHighlight]  = lightGray;
    colors[ImGuiCol_NavWindowingDimBg]      = darkGray;
    colors[ImGuiCol_ModalWindowDimBg]       = almostBlack;
}


void UserInterface::init(
    const Vulkan::Device &device, 
    const Vulkan::Instance &instance,
    const Vulkan::Window &window, 
    const Vulkan::DescriptorPool &descriptorPool,
    const Vulkan::SwapChain &swapChain,
    const Vulkan::GraphicsPipeline &graphicsPipeline)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
    ImFontConfig icons_config; 
    icons_config.MergeMode = true; 
    icons_config.PixelSnapH = true; 
    io.Fonts->AddFontFromFileTTF("assets/fonts/Lato.ttf", 16);
    io.Fonts->AddFontFromFileTTF("assets/fonts/fa-solid-900.ttf", 14, &icons_config, icons_ranges);
    
    ImGui::StyleColorsDark();
    SetDarkTheme();
    
    ImGuiStyle &style = ImGui::GetStyle();
    style.WindowRounding = 2;
    style.GrabRounding = 2;
    style.FrameRounding = 2;
    style.FrameBorderSize = 0;
    style.WindowBorderSize = 0;
    style.DockingSeparatorSize = 1;
    style.WindowPadding = {8, 5};

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

void UserInterface::drawMenuBar(Vulkan::SceneManager &sceneManager) {
    if (ImGui::BeginMainMenuBar()) {

        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem(ICON_FA_FILE_CIRCLE_PLUS"    New"))
                sceneManager.reset();

            if (ImGui::MenuItem(ICON_FA_FOLDER_OPEN"    Open"))
                File::loadSceneFromYAML(pickFilePath(VKPT_SCENE, VKPT_LOAD), sceneManager);

            if (ImGui::MenuItem(ICON_FA_FLOPPY_DISK"      Save"))
                File::saveSceneToYAML(pickFilePath(VKPT_SCENE, VKPT_SAVE), sceneManager);
                
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Add")) {
            if (ImGui::MenuItem(ICON_FA_CIRCLE"    Sphere"))
                sceneManager.addSphere();

            if (ImGui::MenuItem(ICON_FA_DRAW_POLYGON"     Mesh"))
                sceneManager.addMesh(pickFilePath(VKPT_MESH, VKPT_LOAD));

            if (ImGui::MenuItem(ICON_FA_MOUNTAIN_SUN"   Environment"))
                sceneManager.queueEnv(pickFilePath(VKPT_HDRI, VKPT_LOAD));

            ImGui::EndMenu();
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
    const ImGuiViewport *viewport = ImGui::GetMainViewport();

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

    ImGuiID dockspace_id = ImGui::GetID("Dockspace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
    ImGui::End();

    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(2);
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

    if (m_ShowCameraControl) drawCameraControl(sceneManager);
    if (m_ShowObjectControl) drawSceneOverview(sceneManager);
    if (m_ShowStats) drawStats(sceneManager);

    drawMaterialEditor(sceneManager);
    drawObjectEditor(sceneManager);

    ImGuiWindowClass winClass;
    winClass.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoDockingOverMe;
    ImGui::SetNextWindowClass(&winClass);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

    ImGui::Begin("Viewport");
    processCameraMovement(sceneManager);
    position = ImGui::GetWindowPos();
    extent = ImGui::GetWindowSize();
    ImGui::End();

    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(2);

    ImGui::Render();
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
    ImGui::Text("Average: %6.2fms", total / count);
    ImGui::PlotLines("##", m_FrameTimes.data(), m_FrameTimes.size(), 0, nullptr, 0.0f, 2 * total / count, ImVec2(160.0f, 40.0f));

    if (ImGui::Button("Reset Accumulation"))
        sceneManager.resetAccumulation();
    ImGui::End();
}


} // namespace Interface
