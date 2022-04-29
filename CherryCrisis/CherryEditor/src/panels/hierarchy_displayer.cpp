
#include "panels/hierarchy_displayer.hpp"

#include <imgui.h>

#include "scene.hpp"
#include "transform.hpp"
#include "core/editor_manager.hpp"

#include<algorithm>
#include <resource_manager.hpp>
#include "callback.hpp"
#include "scene_manager.hpp"
#include "model.hpp"

#define IMGUI_LEFT_LABEL(func, label, ...) (ImGui::TextUnformatted(label), ImGui::SameLine(), func("##" label, __VA_ARGS__))

template <typename T>
bool Contains(std::vector<T>& vec, const T& elem)
{
    bool result = false;
    if (std::find(vec.begin(), vec.end(), elem) != vec.end())
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
        ImGui::Text(SceneManager::GetInstance()->m_currentScene->GetName().c_str());
        ImGui::Separator();

        for (auto& [entityName, entityRef] : SceneManager::GetInstance()->m_currentScene->m_entities)
        {
            if (entityRef)
            {
                if (Transform* entityTransform = entityRef->GetBehaviour<Transform>();
                    entityTransform && !entityTransform->IsRoot())
                    continue;

                std::string name = entityRef->GetName();
                if (RenderEntity(entityRef))
                    break;
            }
        }
    }

    ContextCallback();

    if (ImGui::IsWindowHovered() && InputManager::GetInstance()->GetKeyDown(Keycode::RIGHT_CLICK))
    {
        ImGui::OpenPopup("context");
    }

    if (m_renaming)
        ImGui::OpenPopup("Rename");

    //TODO: Replace this with per entity for multi selection
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("Rename", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        std::string str = "Renaming ";
        str += m_manager->m_selectedEntities[0]->GetName();
        str += " ? \n\n";
        ImGui::Text(str.c_str());
        ImGui::Separator();

        static char newName[32] = "New Name";
        IMGUI_LEFT_LABEL(ImGui::InputText, "New Name:", newName, IM_ARRAYSIZE(newName));

        ImGui::Separator();

        if (ImGui::Button("Rename", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
            m_manager->m_selectedEntities[0]->SetName(newName);
            memset(newName, 0, sizeof(char) * strlen(newName));
            m_renaming = false;
        }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) { m_renaming = false;  ImGui::CloseCurrentPopup(); }
        ImGui::EndPopup();
    }

    if (ImGui::IsKeyDown(ImGuiKey_F2) && m_manager->m_selectedEntities.size() > 0 && ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows))
    {
        m_renaming = true;
    }

    ImGui::End();

    if (ImGui::IsKeyDown(ImGuiKey_Delete) && m_manager->m_selectedEntities.size() > 0) 
    {
        for (Entity* entity : m_manager->m_selectedEntities) 
        {
            SceneManager::GetInstance()->m_currentScene->RemoveEntity(entity);
            m_manager->m_selectedEntities.clear();
        }
    }

}

bool HierarchyDisplayer::RenderEntity(Entity* entity) 
{
    Transform* entityTransform = entity->GetBehaviour<Transform>();
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

    if (Contains(m_manager->m_selectedEntities, entity))                { flags |= ImGuiTreeNodeFlags_Selected; }
    if (!entityTransform || entityTransform->GetChildren().size() <= 0) { flags |= ImGuiTreeNodeFlags_Leaf; }


    bool opened = ImGui::TreeNodeEx((void*)(intptr_t)entity->GetUUID(), flags, entity->GetName().c_str());
   
    if (InputManager::GetInstance()->GetKeyDown(Keycode::LEFT_CLICK) && ImGui::IsItemHovered()) 
    {
        if (!InputManager::GetInstance()->GetKey(Keycode::LEFT_CONTROL)) // Clear selection when CTRL is not held
            m_manager->m_selectedEntities.clear();

        if (!Contains(m_manager->m_selectedEntities, entity))
            m_manager->m_selectedEntities.push_back(entity);
    }

    if (entityTransform && ImGui::BeginDragDropSource())
    {
        ImGui::SetDragDropPayload("HIERARCHY_DROP", entity, sizeof(Entity), ImGuiCond_Once);
        ImGui::Text(entity->GetName().c_str());
        ImGui::EndDragDropSource();
    }
    Transform* toAdd = nullptr;
    if (entityTransform && ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_DROP"))
        {
            Entity* draggedEntity = (Entity*)payload->Data;

            draggedEntity->GetBehaviour<Transform>()->SetParent(entityTransform);
            m_manager->m_selectedEntities.clear();
            if (opened) ImGui::TreePop();
            return true;
        }

        ImGui::EndDragDropTarget();
    }
    else if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && ImGui::GetCurrentContext()->DragDropActive)
    {
        ImGuiPayload* payload = &ImGui::GetCurrentContext()->DragDropPayload;
        if (payload->IsDataType("HIERARCHY_DROP")) 
        {
            Entity* draggedEntity = static_cast<Entity*>(payload->Data);
            if (draggedEntity)
            {
                Transform* draggedTransform = draggedEntity->GetBehaviour<Transform>();
                if (draggedTransform && draggedTransform == entityTransform)
                {
                    draggedEntity->GetBehaviour<Transform>()->SetParent(nullptr);
                    m_manager->m_selectedEntities.clear();
                    if (opened) ImGui::TreePop();
                    return true;
                }
            }
        }
       
    }

    if (opened && entityTransform)
        for (const auto& child : entityTransform->GetChildren())
            if (RenderEntity(&child->GetHost())) { ImGui::TreePop(); return true; }
        
    if (opened)
        ImGui::TreePop();

    return false;
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
                SceneManager::GetInstance()->m_currentScene->AddEntity(empty);
            }
            if (ImGui::MenuItem("ModelRenderer")) {}
            if (ImGui::MenuItem("Camera")) {}
            if (ImGui::MenuItem("Particle System")) {}
            if (ImGui::MenuItem("Audio Source")) {}
            if (ImGui::BeginMenu("Shapes"))
            {
                if (ImGui::MenuItem("Cube")) 
                {
                    Entity* cube = new Entity("Cube");
                    std::shared_ptr<Mesh> mesh = ResourceManager::GetInstance()->AddResource<Mesh>("CC_NormalizedCube", true, EMeshShape::CUBE, 1, 1, 1);
                    //Mesh::CreateCube(mesh,1,1,1);
                    std::shared_ptr<Material> material = ResourceManager::GetInstance()->AddResource<Material>("CC_MatPBR", true);
                    std::shared_ptr<Model> model = ResourceManager::GetInstance()->AddResource<Model>("CC_NormalizedCube", true, mesh, material);
                    Transform* tr = cube->AddBehaviour<Transform>();
                    ModelRenderer* rdr = cube->AddBehaviour<ModelRenderer>();
                    rdr->m_transform = tr;
                    rdr->SetModel(model);

                    tr->BindToSignals();
                    rdr->BindToSignals();
                    cube->Initialize();
                    
                    SceneManager::GetInstance()->m_currentScene->AddEntity(cube);
                    m_manager->FocusEntity(cube);
                }
                if (ImGui::MenuItem("Sphere")) 
                {
                    Entity* cube = new Entity("Sphere");
                    std::shared_ptr<Mesh> mesh = ResourceManager::GetInstance()->AddResourceRef<Mesh>("CC_NormalizedSphere");
                    Mesh::CreateCube(mesh, 1, 1, 1);
                    std::shared_ptr<Model> model = ResourceManager::GetInstance()->AddResource<Model>("CC_NormalizedSphere", true, mesh);
                    Transform* tr = cube->AddBehaviour<Transform>();
                    ModelRenderer* rdr = cube->AddBehaviour<ModelRenderer>();
                    rdr->m_transform = tr;
                    rdr->SetModel(model);

                    tr->BindToSignals();
                    rdr->BindToSignals();
                    cube->Initialize();

                    SceneManager::GetInstance()->m_currentScene->AddEntity(cube);
                    m_manager->FocusEntity(cube);
                }
                if (ImGui::MenuItem("Cone")) 
                {
                    Entity* cube = new Entity("Cone");
                    std::shared_ptr<Mesh> mesh = ResourceManager::GetInstance()->AddResourceRef<Mesh>("CC_NormalizedCone");
                    Mesh::CreateCube(mesh, 1, 1, 1);
                    std::shared_ptr<Model> model = ResourceManager::GetInstance()->AddResource<Model>("CC_NormalizedCone", true, mesh);
                    Transform* tr = cube->AddBehaviour<Transform>();
                    ModelRenderer* rdr = cube->AddBehaviour<ModelRenderer>();
                    rdr->m_transform = tr;
                    rdr->SetModel(model);
                    SceneManager::GetInstance()->m_currentScene->AddEntity(cube);
                    m_manager->FocusEntity(cube);
                }
                if (ImGui::MenuItem("Plane")) 
                {
                    Entity* cube = new Entity("Plane");
                    std::shared_ptr<Mesh> mesh = ResourceManager::GetInstance()->AddResourceRef<Mesh>("CC_NormalizedPlane");
                    Mesh::CreateCube(mesh, 1, 1, 1);
                    std::shared_ptr<Model> model = ResourceManager::GetInstance()->AddResource<Model>("CC_NormalizedPlane", true, mesh);
                    Transform* tr = cube->AddBehaviour<Transform>();
                    ModelRenderer* rdr = cube->AddBehaviour<ModelRenderer>();
                    rdr->m_transform = tr;
                    rdr->SetModel(model);
                    SceneManager::GetInstance()->m_currentScene->AddEntity(cube);
                    m_manager->FocusEntity(cube);
                }
                ImGui::EndMenu();
            }
            ImGui::Separator();

            ImGui::EndMenu();
        }
        if (m_manager->m_selectedEntities.size() > 0)
        {
            ImGui::Separator();
            

            if (ImGui::MenuItem("Rename")) { m_renaming = true; }
            if (ImGui::MenuItem("Delete")) 
            {
                for (auto& entity : m_manager->m_selectedEntities) 
                {
                    SceneManager::GetInstance()->m_currentScene->RemoveEntity(entity);
                    //To Change
                    m_manager->m_selectedEntities.clear();
                }
            }

            if (ImGui::MenuItem("Copy")) {}

            ImGui::Separator();
        }
        if (ImGui::MenuItem("Paste")) {}

        ImGui::EndPopup();
    }
} 
