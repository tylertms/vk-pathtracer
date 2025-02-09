#include "MaterialEditor.h"

#include <imgui.h>

namespace Interface {

bool drawMaterialEditor(VKPT::Material &material) {
    bool changed = false;

    if (ImGui::ColorEdit3("Base Color", (float*)&material.baseColor)) changed = true;
    // if (ImGui::SliderFloat("Metallic", &material.metallic, 0.0f, 1.0f)) changed = true;
    if (ImGui::SliderFloat("Roughness", &material.roughness, 0.0f, 1.0f)) changed = true;
    if (ImGui::DragFloat2("IOR", (float*)&material.IOR, 0.01f, 0.f, 100.f)) changed = true;
    // if (ImGui::SliderFloat("Alpha", &material.alpha, 0.0f, 1.0f)) changed = true;
    if (ImGui::DragFloat("Specular", &material.specular, 0.01f, 0.0f, 12.5f)) changed = true;
    if (ImGui::ColorEdit3("Specular Tint", (float*)&material.specularTint)) changed = true;
    if (ImGui::DragFloat("Emission Strength", &material.emissionStrength, 0.01f, 0.0f, 1000.f)) changed = true;
    if (ImGui::ColorEdit3("Emission Color", (float*)&material.emissionColor)) changed = true;
    if (ImGui::SliderFloat("Transmission", &material.transmission, 0.0f, 1.0f)) changed = true;
    // if (ImGui::SliderFloat("Transmission Roughness", &material.transmissionRoughness, 0.0f, 1.0f)) changed = true;
    // if (ImGui::SliderFloat("Coat Weight", &material.coatWeight, 0.0f, 1.0f)) changed = true;
    // if (ImGui::SliderFloat("Coat Roughness", &material.coatRoughness, 0.0f, 1.0f)) changed = true;
    // if (ImGui::ColorEdit3("Coat Tint", (float*)&material.coatTint)) changed = true;
    // if (ImGui::DragFloat("Coat IOR", &material.coatIOR, 0.01f, 1.f, 4.f)) changed = true;

    return changed;
}

} // namespace Interface
