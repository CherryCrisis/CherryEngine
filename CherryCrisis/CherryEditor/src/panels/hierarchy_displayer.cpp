
#include "panels/hierarchy_displayer.hpp"

#include <imgui.h>

#include "scene.hpp"
#include "transform.hpp"
#include "light_component.hpp"
#include "camera_component.hpp"

void HierarchyDisplayer::Render() 
{
    if (!m_isOpened)
        return;

    if (ImGui::Begin("Hierarchy", &m_isOpened))
    {
        int count = 0;
        for (auto& [entityName, entity] : m_displayedScene->m_entities)
        {
            Transform* transform;
            
            if (!entity->TryGetBehaviour<Transform>(transform) || !transform->IsRoot())
                continue;

            if (ImGui::TreeNode((void*)(intptr_t)count, "Instance %i", count))
            {
                CameraComponent* cameraComponent;
                if (entity->TryGetBehaviour<CameraComponent>(cameraComponent))
                {
                    Camera& cam = cameraComponent->m_camera;
                    ImGui::DragFloat("Near", &cam.near);
                    ImGui::DragFloat("Far", &cam.far);
                    ImGui::DragFloat("Fov", &cam.fovY);
                }

                LightComponent* lightComponent;
                if (entity->TryGetBehaviour<LightComponent>(lightComponent))
                {
                    Light& light = lightComponent->m_light;
                    ImGui::Checkbox("LightIsPoint", &light.m_isPoint);
                    ImGui::DragFloat3("LightPosition", light.m_position.data, 0.5f);
                    ImGui::DragFloat3("LightAmbient", light.m_ambient.data, 0.5f);
                    ImGui::DragFloat3("LightDiffuse", light.m_diffuse.data, 0.5f);
                    ImGui::DragFloat3("LightSpecular", light.m_specular.data, 0.5f);
                }

                if (transform)
                {
                    Vector3 position = transform->GetPosition();

                    if (ImGui::DragFloat3("Position", position.data, 0.5f))
                        transform->SetPosition(position);

                    Vector3 rotation = transform->GetRotation() * CCMaths::RAD2DEG;

                    if (ImGui::DragFloat3("Rotation", rotation.data, 0.5f))
                        transform->SetRotation(rotation * CCMaths::DEG2RAD);

                    Vector3 scale = transform->GetScale();

                    if (ImGui::DragFloat3("Scale", scale.data, 0.5f))
                        transform->SetScale(scale);
                }

                ImGui::TreePop();
            }

            count++;
        }
    }

    ContextCallback();

    if (ImGui::IsWindowHovered() && ImGui::GetMouseClickedCount(ImGuiMouseButton_Right) == 1)
    {
        ImGui::OpenPopup("context");
    }
    ImGui::End();
}

void HierarchyDisplayer::ContextCallback()
{
    if (ImGui::BeginPopupContextItem("context"))
    {
        ImGui::Text("Actions ...");
        ImGui::Separator();
        if (ImGui::BeginMenu("New"))
        {

            if (ImGui::MenuItem("Empty")) {}
            if (ImGui::MenuItem("ModelRenderer")) {}
            if (ImGui::MenuItem("Camera")) {}
            if (ImGui::MenuItem("Particle System")) {}
            if (ImGui::MenuItem("Audio Source")) {}
            if (ImGui::BeginMenu("Shapes"))
            {
                if (ImGui::MenuItem("Cube")) {}
                if (ImGui::MenuItem("Sphere")) {}
                if (ImGui::MenuItem("Cone")) {}
                if (ImGui::MenuItem("Plane")) {}
                ImGui::EndMenu();
            }
            ImGui::Separator();

            ImGui::EndMenu();
        }
        if (m_focusedEntity)
        {
            ImGui::Separator();
            

            if (ImGui::MenuItem("Rename")) { m_renaming = true; }
            if (ImGui::MenuItem("Delete")) { m_deleting = true; }

            if (ImGui::MenuItem("Copy")) {}

            ImGui::Separator();
        }
        if (ImGui::MenuItem("Paste")) {}

        ImGui::EndPopup();
    }
} 