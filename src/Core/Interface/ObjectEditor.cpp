#include "ObjectEditor.h"
#include "MaterialEditor.h"

#include <imgui.h>

namespace Interface {

bool drawSphereEditor(VKPT::Sphere &sphere) {
    bool changed = false;

    ImGui::Indent();
    if (ImGui::CollapsingHeader("Properties")) {
        if (ImGui::DragFloat("Radius", &sphere.radius, 0.01f)) changed = true;
        if (ImGui::DragFloat3("Center", (float*)&sphere.center, 0.01f)) changed = true;
    }

    if (ImGui::CollapsingHeader("Material")) {
        if (drawMaterialEditor(sphere.material)) changed = true;
    }
    ImGui::Unindent();

    return changed;
}

bool drawMeshEditor(VKPT::Mesh &mesh, glm::mat3 &transform) {
    bool changed = false;

    ImGui::Indent();
    if (ImGui::CollapsingHeader("Properties")) {
        if (ImGui::DragFloat3("Translation", (float*)&transform[0], 0.01f)) changed = true;
        if (ImGui::DragFloat3("Rotation", (float*)&transform[1], 0.5f)) changed = true;
        if (ImGui::DragFloat3("Scale", (float*)&transform[2], 0.01f)) changed = true;
    }

    if (ImGui::CollapsingHeader("Material")) {
        if (drawMaterialEditor(mesh.material)) changed = true;
    }
    ImGui::Unindent();

    return changed;
}

void drawObjectControl(Vulkan::SceneManager &sceneManager) {
    ImGui::Begin("Objects");

    for (uint32_t i = 0; i < sceneManager.sceneData.numSpheres; i++) {
        ImGui::PushID("Sphere");
        ImGui::PushID(i);
        if (ImGui::CollapsingHeader("Sphere", ImGuiTreeNodeFlags_DefaultOpen)) {
            if (Interface::drawSphereEditor(sceneManager.sceneData.spheres[i]))
                sceneManager.resetAccumulation();
        }
        ImGui::PopID();
        ImGui::PopID();
    }

    for (uint32_t i = 0; i < sceneManager.sceneData.numMeshes; i++) {
        ImGui::PushID("Mesh");
        ImGui::PushID(i);
        if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen)) {
            if (Interface::drawMeshEditor(sceneManager.sceneData.meshes[i],
                                          sceneManager.meshTransforms[i])) {
                sceneManager.updateMeshTransforms();
                sceneManager.resetAccumulation();
            }
        }
        ImGui::PopID();
        ImGui::PopID();
    }

    ImGui::End();
}

} // namespace Interface
