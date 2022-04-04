
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
        ImGui::BeginChild("DragDropTarget");
        for (auto& [entityName, entityRef] : m_displayedScene->m_entities)
        {
            if (Transform* entityTransform = entityRef->GetBehaviour<Transform>();
                entityTransform && !entityTransform->IsRoot())
                continue;
            
            ImGui::PushID(entityRef->GetUUID());
            std::string name = entityRef->GetName();
            RenderEntity(entityRef);
            
            ImGui::PopID();
        }

        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(".obj"))
            {
                const char* c = (const char*)payload->Data;
                std::string str = "Assets/" + std::string(c);
                const char* strReal = str.c_str();
            }


            ImGui::EndDragDropTarget();
        }
        ImGui::EndChild();
    }

    ContextCallback();

    if (ImGui::IsWindowHovered() && ImGui::GetMouseClickedCount(ImGuiMouseButton_Right) == 1)
    {
        ImGui::OpenPopup("context");
    }

    
    ImGui::End();
}

void HierarchyDisplayer::RenderEntity(Entity* entity) 
{
    Transform* entityTransform = entity->GetBehaviour<Transform>();
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

    if (contains(m_manager->m_selectedEntities, entity))                { flags |= ImGuiTreeNodeFlags_Selected; }
    if (!entityTransform || entityTransform->GetChildren().size() <= 0) { flags |= ImGuiTreeNodeFlags_Leaf; }


    bool opened = ImGui::TreeNodeEx((void*)(intptr_t)entity->GetUUID(), flags, entity->GetName().c_str());
   
    if (InputManager::GetInstance()->GetKeyDown(Keycode::LEFT_CLICK) && ImGui::IsItemHovered()) 
    {
        if (!InputManager::GetInstance()->GetKey(Keycode::LEFT_CONTROL))    // Clear selection when CTRL is not held
            m_manager->m_selectedEntities.clear();

        if (!contains(m_manager->m_selectedEntities, entity))
            m_manager->m_selectedEntities.push_back(entity);
    }

    if (opened && entityTransform)
        for (const auto& child : entityTransform->GetChildren())
            RenderEntity(&child->GetHost());
        
    if (opened)
        ImGui::TreePop();
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
                if (ImGui::MenuItem("Cube")) 
                {
                    Entity* cube = new Entity("Cube");
                    std::shared_ptr<Mesh> mesh = ResourceManager::GetInstance()->AddResourceRef<Mesh>("CC_NormalizedCube");
                    Mesh::CreateCube(mesh,1,1,1);
                    std::shared_ptr<Model> model = ResourceManager::GetInstance()->AddResource<Model>("CC_NormalizedCube",true,mesh);
                    Transform* tr = cube->AddBehaviour<Transform>();
                    ModelRenderer* rdr = cube->AddBehaviour<ModelRenderer>();
                    rdr->m_transform = tr;
                    rdr->SetModel(model);
                    m_displayedScene->AddEntity(cube);
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
                    m_displayedScene->AddEntity(cube);
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
                    m_displayedScene->AddEntity(cube);
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
                    m_displayedScene->AddEntity(cube);
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
                    m_displayedScene->RemoveEntity(entity);
                    entity->Destroy();
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
