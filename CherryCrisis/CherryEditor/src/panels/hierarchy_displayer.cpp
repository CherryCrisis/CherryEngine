
#include "panels/hierarchy_displayer.hpp"

#include <imgui.h>
#include <imgui_internal.h>
#include <algorithm>

#include "resource_manager.hpp"
#include "scene_manager.hpp"

#include "scene.hpp"
#include "transform.hpp"
#include "portal_component.hpp"
#include "core/editor_manager.hpp"
#include "callback.hpp"
#include "cell.hpp"

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

HierarchyDisplayer::HierarchyDisplayer(bool spawnOpened, EditorManager* manager) 
    : Panel(spawnOpened), m_manager(manager) 
{
    if (m_manager)
        m_cellSystemDisplayer = m_manager->GetCellSystemDisplayer();
}

void HierarchyDisplayer::Render()
{
    if (!m_isOpened)
        return;

    if (ImGui::Begin("Hierarchy", &m_isOpened))
    {
        ImGui::Text(SceneManager::GetInstance()->m_currentScene->GetName().c_str());

        Cell* selectedCell = m_cellSystemDisplayer->GetSelectedCell();

        if (!selectedCell)
        {
            ImGui::End();
            return;
        }

        ImGui::Text(std::format("Cell : {}", selectedCell->GetName().c_str()).c_str());
        ImGui::Separator();

        auto entities = selectedCell->GetEntities();
        for (auto& entity : entities)
        {
            if (entity)
            {
                if (Transform* entityTransform = entity->GetBehaviour<Transform>();
                    entityTransform && !entityTransform->IsRoot())
                    continue;

                std::string name = entity->GetName();
                if (RenderEntity(entity))
                    break;
            }
        }

        if (ImGui::IsWindowHovered())
        {
            if (InputManager::GetKeyDown(Keycode::RIGHT_CLICK))
            {
                ImGui::OpenPopup("context");
            }

            if (!m_manager->m_entitySelector.IsEmpty())
            {
                if (InputManager::GetKeyDown(Keycode::F2))
                    m_renaming = true;

                if (InputManager::GetKeyDown(Keycode::DEL))
                {
                    for (Entity* entity : m_manager->m_entitySelector.m_entities)
                        SceneManager::GetInstance()->m_currentScene->RemoveEntity(entity);

                    m_manager->m_entitySelector.Clear();

                    ImGui::End();
                    return;

                }
            }
        }

        ContextCallback();
    }
    ImGui::End();


    if (m_renaming)
        ImGui::OpenPopup("Rename");

    //TODO: Replace this with per entity for multi selection
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("Rename", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        std::string str = "Renaming ";
        str += m_manager->m_entitySelector.m_entities[0]->GetName();
        str += " ? \n\n";
        ImGui::Text(str.c_str());
        ImGui::Separator();

        static char newName[32];
        IMGUI_LEFT_LABEL(ImGui::InputText, "New Name:", newName, IM_ARRAYSIZE(newName));
        ImGui::Separator();

        if (ImGui::Button("Rename", ImVec2(120, 0)) && std::string(newName) != "")
        {
            ImGui::CloseCurrentPopup();
            m_manager->m_entitySelector.m_entities[0]->SetName(newName);
            memset(newName, 0, sizeof(char) * strlen(newName));
            m_renaming = false;
        }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) { m_renaming = false;  ImGui::CloseCurrentPopup(); }
        ImGui::EndPopup();
    }

}

bool HierarchyDisplayer::RenderEntity(Entity* entity)
{
    EntitySelector* selector = &m_manager->m_entitySelector;

    Transform* entityTransform = entity->GetBehaviour<Transform>();
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

    if (selector->Contains(entity))                                     flags |= ImGuiTreeNodeFlags_Selected;
    if (!entityTransform || entityTransform->GetChildren().size() <= 0) flags |= ImGuiTreeNodeFlags_Leaf;

    bool opened = ImGui::TreeNodeEx((void*)(intptr_t)entity->GetUUID(), flags, entity->GetName().c_str());

    if (entityTransform && ImGui::BeginDragDropSource())
    {
        ImGui::SetDragDropPayloadNoCopy("HIERARCHY_DROP", entity, sizeof(Entity), ImGuiCond_Once);
        ImGui::Text(entity->GetName().c_str());
        ImGui::EndDragDropSource();
        m_isEntityDragged = true;
    }

    if (InputManager::GetKeyDown(Keycode::LEFT_CLICK) && ImGui::IsItemHovered() && !m_isEntityDragged)
    {
        if (InputManager::GetKey(Keycode::LEFT_CONTROL))
        {
            if (!selector->Contains(entity))
                selector->Add(entity);
            else
                selector->Remove(entity);
        }
        else if (InputManager::GetKey(Keycode::LEFT_SHIFT))
        {
            selector->SetEndRange();
            selector->SetStartRange(entity);
            selector->ApplyRange();
        }
        else
        {
            selector->Clear();
            selector->Add(entity);
        }

        selector->SetStartRange(entity);
    }

    if (entityTransform && ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_DROP"))
        {
            m_draggedEntity = static_cast<Entity*>(payload->Data);
            m_draggedEntity->GetBehaviour<Transform>()->SetParent(entityTransform, true, true, true);
            m_manager->m_entitySelector.Clear();
            m_isEntityDragged = false;
            m_draggedEntity = nullptr;
            if (opened) ImGui::TreePop();
            return true;
        }

        ImGui::EndDragDropTarget();
    }

    else if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && ImGui::GetCurrentContext()->DragDropActive && ImGui::IsWindowHovered())
    {
        ImGuiPayload* payload = &ImGui::GetCurrentContext()->DragDropPayload;
        if (payload->IsDataType("HIERARCHY_DROP"))
        {
            m_draggedEntity = static_cast<Entity*>(payload->Data);
        }
    }
    else if (m_draggedEntity)
    {
        Transform* draggedTransform = m_draggedEntity->GetBehaviour<Transform>();
        if (draggedTransform && draggedTransform == entityTransform)
        {
            if (m_isEntityDragged)
            {
                m_isEntityDragged = false;
                m_draggedEntity = entity;
            }
            else
            {
                m_draggedEntity->GetBehaviour<Transform>()->SetParent(nullptr, true, true, true);
                m_manager->m_entitySelector.Clear();
                m_manager->m_entitySelector.Add(m_draggedEntity);
                m_draggedEntity = nullptr;
                if (opened) ImGui::TreePop();
                return true;
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
            Entity* newEntity = nullptr;

            if (ImGui::MenuItem("Empty"))
            {
                if (Cell* cell = m_cellSystemDisplayer->GetSelectedCell())
                {
                    newEntity = new Entity("Empty", cell);
                }
            }

            if (ImGui::MenuItem("ModelRenderer")) {}
            if (ImGui::BeginMenu("Portal"))
            {
                if (ImGui::MenuItem("One Portal"))
                {
                    if (Cell* cell = m_cellSystemDisplayer->GetSelectedCell())
                    {
                        newEntity = new Entity("Portal", cell);
                    }

                    newEntity->AddBehaviour<PortalComponent>();
                }

                if (ImGui::MenuItem("Pair Portals"))
                {
                    if (Cell* cell = m_cellSystemDisplayer->GetSelectedCell())
                    {
                        Scene* scene = SceneManager::GetInstance()->m_currentScene.get();
                        Entity* entity_0 = new Entity("Portal_0", cell);

                        Entity* entity_1 = new Entity("Portal_1", cell);

                        entity_0->AddBehaviour<Transform>();
                        entity_1->AddBehaviour<Transform>();

                        PortalComponent* portal_0 = entity_0->AddBehaviour<PortalComponent>();
                        PortalComponent* portal_1 = entity_1->AddBehaviour<PortalComponent>();

                        entity_0->Initialize();
                        entity_1->Initialize();

                        scene->AddEntity(entity_0);
                        m_manager->FocusEntity(entity_0);

                        scene->AddEntity(entity_1);
                        m_manager->FocusEntity(entity_1);

                        portal_0->SetLinkedPortal(entity_1);
                        portal_1->SetLinkedPortal(entity_0);

                    }
                }

                ImGui::EndMenu();
            }

            if (ImGui::MenuItem("Camera")) {}
            if (ImGui::MenuItem("Particle System")) {}
            if (ImGui::MenuItem("Audio Source")) {}
            if (ImGui::BeginMenu("Shapes"))
            {
                Entity* newEntity = nullptr;
               /* std::shared_ptr<Model> newModel = nullptr;
                if (ImGui::MenuItem("Cube"))
                {
                    newEntity = new Entity("Cube");
                    std::shared_ptr<Mesh> mesh = ResourceManager::GetInstance()->AddResourceRef<Mesh>("CC_NormalizedCube");
                    std::shared_ptr<Material> material = ResourceManager::GetInstance()->AddResourceRef<Material>("CC_Mat");
                    Mesh::CreateCube(mesh, 1, 1, 1);
                    newModel = ResourceManager::GetInstance()->AddResource<Model>("CC_NormalizedCube", true, mesh);
                }

                if (ImGui::MenuItem("Sphere"))
                {
                    newEntity = new Entity("Sphere");
                    std::shared_ptr<Mesh> mesh = ResourceManager::GetInstance()->AddResourceRef<Mesh>("CC_NormalizedSphere");
                    Mesh::CreateCube(mesh, 1, 1, 1);
                    newModel = ResourceManager::GetInstance()->AddResource<Model>("CC_NormalizedSphere", true, mesh);
                }
                if (ImGui::MenuItem("Cone"))
                {
                    newEntity = new Entity("Cone");
                    std::shared_ptr<Mesh> mesh = ResourceManager::GetInstance()->AddResourceRef<Mesh>("CC_NormalizedCone");
                    Mesh::CreateCube(mesh, 1, 1, 1);
                    newModel = ResourceManager::GetInstance()->AddResource<Model>("CC_NormalizedCone", true, mesh);
                }
                if (ImGui::MenuItem("Plane"))
                {
                    newEntity = new Entity("Plane");
                    std::shared_ptr<Mesh> mesh = ResourceManager::GetInstance()->AddResourceRef<Mesh>("CC_NormalizedPlane");
                    Mesh::CreateQuad(mesh, 1, 1);
                    newModel = ResourceManager::GetInstance()->AddResource<Model>("CC_NormalizedPlane", true, mesh);
                }

                if (newEntity && newModel)
                {
                    std::shared_ptr<Material> material = ResourceManager::GetInstance()->AddResourceRef<Material>("CC_Mat");

                    newModel->SetMaterial(material);
                    Transform* tr = newEntity->AddBehaviour<Transform>();
                    ModelRenderer* rdr = newEntity->AddBehaviour<ModelRenderer>();
                    rdr->m_transform = tr;
                    rdr->SetModel(newModel);

                    newEntity->Initialize();

                    SceneManager::GetInstance()->m_currentScene->AddEntity(newEntity);
                    m_manager->FocusEntity(newEntity);
                }*/

                ImGui::EndMenu();
            }

            if (newEntity)
            {
                SceneManager::GetInstance()->m_currentScene->AddEntity(newEntity);
                m_manager->FocusEntity(newEntity);
            }

            ImGui::Separator();

            ImGui::EndMenu();
        }
        if (!m_manager->m_entitySelector.IsEmpty())
        {
            ImGui::Separator();

            if (ImGui::MenuItem("Rename")) { m_renaming = true; }
            if (ImGui::MenuItem("Delete"))
            {
                for (auto& entity : m_manager->m_entitySelector.m_entities)
                {
                    SceneManager::GetInstance()->m_currentScene->RemoveEntity(entity);
                }
                //To Change
                m_manager->m_entitySelector.Clear();
            }

            if (ImGui::MenuItem("Copy")) {}

            ImGui::Separator();
        }
        if (ImGui::MenuItem("Paste")) {}

        ImGui::EndPopup();
    }
}
