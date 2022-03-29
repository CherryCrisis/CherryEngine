
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
            if (Transform* entityTransform = entityRef->GetBehaviour<Transform>();
                entityTransform && !entityTransform->IsRoot())
                continue;

            std::string name = entityRef->GetName();

            if (ImGui::Selectable(name.c_str(), contains(m_manager->m_selectedEntities, entityRef)))
            {
                if (!ImGui::GetIO().KeyCtrl)    // Clear selection when CTRL is not held
                    m_manager->m_selectedEntities.clear();

                if (!contains(m_manager->m_selectedEntities, entityRef))
                    m_manager->m_selectedEntities.push_back(entityRef);
            }

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
        if (ImGui::Button("Load"))
        {
            if (m_displayedScene->Unserialize(m_displayedScene->GetFilepath()))
            {
                EditorManager::SendNotification("Scene  Load !", ImGuiToastType::Success, 2.f);
            }
            else
            {
                EditorManager::SendNotification("Scene failed to load.", ImGuiToastType::Error, 2.f);
            }
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

            if (ImGui::MenuItem("Empty")) 
            {
                Entity* empty = new Entity("Empty");
                m_displayedScene->AddEntity(empty);
            }
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
