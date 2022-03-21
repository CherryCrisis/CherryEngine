
#include "panels/hierarchy_displayer.hpp"

#include <imgui.h>

#include "scene.hpp"
#include "transform.hpp"
#include "core/editor_manager.hpp"

#include<algorithm>

template <typename T>
bool contains(std::vector<T> vec, const T& elem)
{
    bool result = false;
    if (find(vec.begin(), vec.end(), elem) != vec.end())
    {
        result = true;
    }
    return result;
}

void HierarchyDisplayer::Render() 
{
    if (!m_isOpened)
        return;

    if (ImGui::Begin("Hierarchy", &m_isOpened))
    {
        int count = 0;
        for (auto& [entityName, entityRef] : m_displayedScene->m_entities)
        {
            Transform* entityTransform = entityRef->GetBehaviour<Transform>();

            if (!entityTransform || !entityTransform->IsRoot())
                continue;

            std::string name = "Instance " + std::to_string(count);

            if (ImGui::Selectable(name.c_str(), contains(m_manager->m_selectedEntities, entityRef)))
            {
                if (!ImGui::GetIO().KeyCtrl)    // Clear selection when CTRL is not held
                    m_manager->m_selectedEntities.clear();

                if (!contains(m_manager->m_selectedEntities, entityRef))
                    m_manager->m_selectedEntities.push_back(entityRef);
            }

            /*if (ImGui::TreeNode((void*)(intptr_t)i, "Instance %i", i))
            {
                if (entity.m_cameraComp)
                {
                    Camera& cam = entity.m_cameraComp->m_camera;
                    ImGui::DragFloat("Near", &cam.near);
                    ImGui::DragFloat("Far", &cam.far);
                    ImGui::DragFloat("Fov", &cam.fovY);
                }

                if (entity.m_lightComp)
                {
                    Light& light = entity.m_lightComp->m_light;
                    ImGui::Checkbox("LightIsPoint", &light.m_isPoint);
                    ImGui::DragFloat3("LightPosition", light.m_position.data, 0.5f);
                    ImGui::DragFloat3("LightAmbient", light.m_ambient.data, 0.5f);
                    ImGui::DragFloat3("LightDiffuse", light.m_diffuse.data, 0.5f);
                    ImGui::DragFloat3("LightSpecular", light.m_specular.data, 0.5f);
                }

                Vector3 position = entity.m_transform->GetPosition();

                if (ImGui::DragFloat3("Position", position.data, 0.5f))
                    entity.m_transform->SetPosition(position);

                Vector3 rotation = entity.m_transform->GetRotation() * CCMaths::RAD2DEG;

                if (ImGui::DragFloat3("Rotation", rotation.data, 0.5f))
                    entity.m_transform->SetRotation(rotation * CCMaths::DEG2RAD);

                Vector3 scale = entity.m_transform->GetScale();

                if (ImGui::DragFloat3("Scale", scale.data, 0.5f))
                    entity.m_transform->SetScale(scale);


                ImGui::TreePop();
            }*/
            count++;
        }
        if (ImGui::Button("Save")) 
        { 
            if (m_displayedScene->Serialize(m_displayedScene->GetFilepath())) 
            {
                EditorManager::SendNotification("Scene  Saved !", ImGuiToastType::Success, 2.f);
            } 
            else 
            {
                EditorManager::SendNotification("Scene failed to save.", ImGuiToastType::Error, 2.f);
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Load")) { }
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
