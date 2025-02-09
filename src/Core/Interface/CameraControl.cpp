#include "CameraControl.h"

#include <imgui.h>

namespace Interface {

void processCameraMovement(Vulkan::SceneManager &sceneManager) {
    VKPT::Camera &cam = sceneManager.sceneData.camera;
    const glm::vec3 WORLD_UP(0.0f, -1.0f, 0.0f);
    ImGuiIO &io = ImGui::GetIO();
    ImVec2 mouseDelta = io.MouseDelta;
    float scroll = io.MouseWheel * ZOOM_SPEED;
    const float minDistance = 0.01f;
    const float maxDistance = 1000.f;
    glm::vec3 lookAt = cam.lookAt;
    glm::vec3 lookFrom = cam.lookFrom;
    glm::vec3 viewDir = lookAt - lookFrom;
    float currentDist = glm::length(viewDir);
    float newDistance = glm::clamp(currentDist - scroll, minDistance, maxDistance);
    if (newDistance != currentDist && ImGui::IsWindowHovered()) {
        glm::vec3 normDir = glm::normalize(viewDir);
        lookFrom = lookAt - normDir * newDistance;
        cam.lookFrom = lookFrom;
        sceneManager.resetAccumulation();
    }
    glm::vec3 right = glm::normalize(glm::cross(viewDir, WORLD_UP));
    glm::vec3 up = glm::normalize(glm::cross(right, viewDir));
    if (ImGui::IsMouseDragging(ImGuiMouseButton_Right) && (ImGui::IsWindowHovered() || ImGui::IsWindowFocused())) {
        ImGui::SetWindowFocus();
        glm::vec2 delta(mouseDelta.x * PAN_SPEED, -mouseDelta.y * PAN_SPEED);
        glm::vec3 move = right * delta.x + up * delta.y;
        lookFrom += move;
        lookAt += move;
        sceneManager.resetAccumulation();
    }
    if (ImGui::IsMouseDragging(ImGuiMouseButton_Left) && ImGui::IsWindowFocused()) {
        glm::vec2 delta(-mouseDelta.x * ORBIT_SPEED, mouseDelta.y * ORBIT_SPEED);
        glm::vec3 offset = lookFrom - lookAt;
        float radius = glm::length(offset);
        glm::vec3 corrected = glm::vec3(offset.x, -offset.y, offset.z);
        float theta = atan2(corrected.x, corrected.z);
        float phi = acos(glm::clamp(corrected.y / radius, -1.0f, 1.0f));
        theta += delta.x;
        phi = glm::clamp(phi + delta.y, 0.01f, glm::pi<float>() - 0.01f);
        float sinPhi = sin(phi);
        glm::vec3 newOffset = glm::vec3(radius * sinPhi * sin(theta),
                                        -radius * cos(phi),
                                        radius * sinPhi * cos(theta));
        lookFrom = lookAt + newOffset;
        sceneManager.resetAccumulation();
    }
    cam.lookFrom = lookFrom;
    cam.lookAt   = lookAt;
}

void drawCameraControl(Vulkan::SceneManager &sceneManager) {
    ImGui::Begin("Camera");
    auto &camera = sceneManager.sceneData.camera;
    bool reset = false;

    uint32_t samplesMin = 1, samplesMax = 100;
    uint32_t bouncesMin = 0, bouncesMax = 30;

    if (ImGui::DragScalar("Samples Per Pixel", ImGuiDataType_U32, &camera.samplesPerPixel, 1, &samplesMin, &samplesMax)) { }
    if (ImGui::DragScalar("Max Bounces", ImGuiDataType_U32, &camera.maxBounces, 1, &bouncesMin, &bouncesMax)) reset = true;
    if (ImGui::DragFloat("vFOV", &camera.vfov, 0.1f, 10.f, 120.f)) reset = true;
    if (ImGui::DragFloat("Exposure", &camera.exposure, 0.01f, 0.01f, 1000.f)) reset = true;
    if (ImGui::DragFloat("Focal Distance", &camera.focalDistance, 0.01f, 0.1f, 100.f)) reset = true;
    if (ImGui::DragFloat("Focal Strength", &camera.defocus, 0.1f, 0.f, 1000.f)) reset = true;
    if (ImGui::DragFloat("Environment Rotation", &camera.envRotation, 0.5f)) reset = true;
    ImGui::End();

    if (reset) sceneManager.resetAccumulation();
}

}