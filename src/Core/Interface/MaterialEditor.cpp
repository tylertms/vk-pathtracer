#include "MaterialEditor.h"
#include "ObjectEditor.h"

#include <glm/glm.hpp>
#include <imgui.h>
#include <icons/IconsFontAwesome6.h>
#include <string>

namespace Interface {

void drawMaterialEditor(Vulkan::SceneManager &sceneManager) {
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

    ImGui::Begin("Material");

    if (sceneManager.sceneData.numMaterials > 0 && objectMatIndex < sceneManager.sceneData.numMaterials) {
        VKPT::Material &material = sceneManager.sceneData.materials[objectMatIndex];
        bool changed = false;

        if (ImGui::ColorEdit3("Base Color", glm::value_ptr(material.baseColor))) changed = true;
        if (ImGui::SliderFloat("Roughness", &material.roughness, 0.0f, 1.0f)) changed = true;
        if (ImGui::DragFloat2("IOR", glm::value_ptr(material.IOR), 0.01f, 0.f, 100.f)) changed = true;
        if (ImGui::DragFloat("Specular", &material.specular, 0.01f, 0.0f, 12.5f)) changed = true;
        if (ImGui::ColorEdit3("Specular Tint", glm::value_ptr(material.specularTint))) changed = true;
        if (ImGui::DragFloat("Emission Strength", &material.emissionStrength, 0.01f, 0.0f, 1000.f)) changed = true;
        if (ImGui::ColorEdit3("Emission Color", glm::value_ptr(material.emissionColor))) changed = true;
        if (ImGui::SliderFloat("Transmission", &material.transmission, 0.0f, 1.0f)) changed = true;

        if (changed) sceneManager.resetAccumulation();
    }

    ImGui::End();
}

} // namespace Interface
