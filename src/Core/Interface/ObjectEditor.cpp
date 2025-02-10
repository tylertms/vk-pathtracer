#include "ObjectEditor.h"
#include "MaterialEditor.h"
#include "../File/SceneLoader.h"

#include <imgui.h>
#include <icons/IconsFontAwesome6.h>

namespace Interface {

bool drawSphereEditor(VKPT::Sphere &sphere) {
    bool changed = false;

    if (ImGui::DragFloat3("Center", (float*)&sphere.center, 0.01f)) changed = true;
    if (ImGui::DragFloat("Radius", &sphere.radius, 0.01f)) changed = true;

    return changed;
}

bool drawMeshEditor(VKPT::Mesh &mesh, glm::mat3 &transform) {
    bool changed = false;

    if (ImGui::DragFloat3("Translation", (float*)&transform[0], 0.01f)) changed = true;
    if (ImGui::DragFloat3("Rotation", (float*)&transform[1], 0.5f)) changed = true;
    if (ImGui::DragFloat3("Scale", (float*)&transform[2], 0.01f)) changed = true;

    return changed;
}

void drawSceneOverview(Vulkan::SceneManager &sceneManager) {
    ImGui::Begin("Scene");
    for (uint32_t i = 0; i < sceneManager.sceneData.numSpheres; i++) {
        bool selected = (sceneManager.selectedObjectType == VKPT_SPHERE) && (sceneManager.selectedObjectIndex == i);
        std::string str = ICON_FA_CIRCLE"     Sphere - " + std::to_string(i + 1);

        if (ImGui::Selectable(str.c_str(), selected)) {
            if (selected) {
                sceneManager.selectedObjectIndex = -1;
            } else {
                sceneManager.selectedObjectIndex = i;
                sceneManager.selectedObjectType = VKPT_SPHERE;
            }
        }
    }

    for (uint32_t i = 0; i < sceneManager.sceneData.numMeshes; i++) {
        bool selected = (sceneManager.selectedObjectType == VKPT_MESH) && (sceneManager.selectedObjectIndex == i);
        std::string str = ICON_FA_DRAW_POLYGON"      Mesh - " +  File::extractFilename(sceneManager.modelPaths[i]);

        if (ImGui::Selectable(str.c_str(), selected)) {
            if (selected) {
                sceneManager.selectedObjectIndex = -1;
            } else {
                sceneManager.selectedObjectIndex = i;
                sceneManager.selectedObjectType = VKPT_MESH;
            }
        }
    }

    ImGui::End();
}

void drawObjectEditor(Vulkan::SceneManager &sceneManager) {
    const int selectedObjIndex = sceneManager.selectedObjectIndex;
    if (selectedObjIndex == -1)
        return;

    auto getObjectMaterialIndex = [&]() -> uint32_t& {
        if (sceneManager.selectedObjectType == VKPT_SPHERE)
            return sceneManager.sceneData.spheres[selectedObjIndex].materialIndex;
        else if (sceneManager.selectedObjectType == VKPT_MESH)
            return sceneManager.sceneData.meshes[selectedObjIndex].materialIndex;
        else {
            static uint32_t dummy = 0;
            return dummy;
        }
    };
    uint32_t &objectMatIndex = getObjectMaterialIndex();

    ImGui::Begin("Object");

    if (sceneManager.selectedObjectType == VKPT_SPHERE) {
        if (drawSphereEditor(sceneManager.sceneData.spheres[selectedObjIndex]))
            sceneManager.resetAccumulation();
    } else if (sceneManager.selectedObjectType == VKPT_MESH) {
        if (drawMeshEditor(sceneManager.sceneData.meshes[selectedObjIndex], sceneManager.meshTransforms[selectedObjIndex])) {
            sceneManager.updateMeshTransforms();
            sceneManager.resetAccumulation();
        }
    }

    std::string comboLabel = (objectMatIndex < sceneManager.sceneData.numMaterials) ? "Material " + std::to_string(objectMatIndex) : "None";

    if (ImGui::BeginCombo("Material", comboLabel.c_str())) {
        for (uint32_t i = 0; i < sceneManager.sceneData.numMaterials; i++) {
            VKPT::Material &mat = sceneManager.sceneData.materials[i];
            std::string labelStr = "Material " + std::to_string(i);
            bool isSelected = (objectMatIndex == i);
            ImGui::PushID(i);
        
            if (ImGui::Selectable("##dummy", isSelected)) {
                objectMatIndex = i;
                sceneManager.resetAccumulation();
            }
        
            ImVec2 rectMin = ImGui::GetItemRectMin();
            ImVec2 rectMax = ImGui::GetItemRectMax();
        
            ImVec2 iconSize  = ImGui::CalcTextSize(ICON_FA_CIRCLE);
            ImVec2 labelSize = ImGui::CalcTextSize(labelStr.c_str());
            float padding    = 4.0f;
        

            ImVec2 iconPos = ImVec2(
                rectMin.x + padding,
                rectMin.y + ((rectMax.y - rectMin.y) - iconSize.y) * 0.5f
            );

            ImVec2 labelPos = ImVec2(
                iconPos.x + iconSize.x + padding,
                rectMin.y + ((rectMax.y - rectMin.y) - labelSize.y) * 0.5f
            );
        
            ImGui::GetWindowDrawList()->AddText(
                iconPos, 
                ImGui::GetColorU32(ImVec4(mat.baseColor.r, mat.baseColor.g, mat.baseColor.b, 1.0f)),
                ICON_FA_CIRCLE
            );

            ImGui::GetWindowDrawList()->AddText(
                labelPos, 
                ImGui::GetColorU32(ImGui::GetStyleColorVec4(ImGuiCol_Text)),
                labelStr.c_str()
            );
        
            ImGui::PopID();
        }
        

        if (ImGui::Button("Add New")) {
            uint32_t newMaterialIndex = sceneManager.sceneData.numMaterials++;
            if (sceneManager.selectedObjectType == VKPT_MESH)
                sceneManager.sceneData.meshes[selectedObjIndex].materialIndex = newMaterialIndex;
            else if (sceneManager.selectedObjectType == VKPT_SPHERE)
                sceneManager.sceneData.spheres[selectedObjIndex].materialIndex = newMaterialIndex;

            objectMatIndex = newMaterialIndex;
            sceneManager.resetAccumulation();
        }

        ImGui::EndCombo();
    }

    ImGui::End();
}

} // namespace Interface
