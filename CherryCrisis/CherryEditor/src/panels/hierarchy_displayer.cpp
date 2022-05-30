#include "panels/hierarchy_displayer.hpp"

#include <algorithm>

#include <imgui.h>
#include <imgui_internal.h>

#include "resource_manager.hpp"
#include "scene_manager.hpp"

#include "audio_emitter.hpp"
#include "audio_listener.hpp"
#include "box_collider.hpp"
#include "callback.hpp"
#include "camera_component.hpp"
#include "capsule_collider.hpp"
#include "cell.hpp"
#include "core/editor_manager.hpp"
#include "core/imcherry.hpp"
#include "light_component.hpp"
#include "mesh.hpp"
#include "scene.hpp"
#include "shape_renderer.hpp"
#include "sphere_collider.hpp"
#include "transform.hpp"
#include "portal_component.hpp"
#include "sky_renderer.hpp"

#define IMGUI_LEFT_LABEL(func, label, ...) (ImGui::TextUnformatted(label), ImGui::SameLine(), func("##" label, __VA_ARGS__))

HierarchyDisplayer::HierarchyDisplayer(bool spawnOpened, EditorManager* manager) 
    : Panel(spawnOpened), m_manager(manager) 
{
    if (m_manager)
        m_cellSystemDisplayer = m_manager->GetCellSystemDisplayer();

    IO = &ImGui::GetIO();
}

void HierarchyDisplayer::EmptyDrop() 
{
    m_isDragging = false;
    m_draggedEntity = nullptr;
}

void HierarchyDisplayer::HandleShortcuts()
{
    if (ImGui::IsWindowHovered())
    {
        if (!m_manager->m_entitySelector.IsEmpty())
        {
            if (InputManager::GetKeyDown(Keycode::F2))
                m_isRenaming = true;

            if (InputManager::GetKeyDown(Keycode::DEL))
            {
                for (Entity* entity : m_manager->m_entitySelector.m_entities)
                    SceneManager::GetInstance()->m_currentScene->RemoveEntity(entity);

                SceneManager::SetHierarchyDirty(true);

                m_manager->m_entitySelector.Clear(false);
            }
        }
    }
}

void HierarchyDisplayer::Render()
{
    if (!m_isOpened)
        return;

    if (!IO)
    {
        IO = &ImGui::GetIO();
        return;
    }
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.f,5.f });
    if (ImGui::Begin("Hierarchy", &m_isOpened))
    {
        if (InputManager::GetKeyDown(Keycode::RIGHT_CLICK) && ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows))
            ImGui::OpenPopup("context");
        
        if (ImGui::BeginPopupContextItem("context"))
            ContextCallback();
        
        // fps displayer
        if (0)
        {
            static std::vector<float> m_fpsHistory;
            static int range = 100;

            static float recordRemainTime = 1.f; // seconds
            static float recordRange      = .05f; // seconds
            recordRemainTime -= TimeManager::GetDeltaTime();
            if ( recordRemainTime <= 0.f)
            {
                m_fpsHistory.push_back(IO->Framerate);

                if (m_fpsHistory.size() > range) 
                    m_fpsHistory.erase(m_fpsHistory.begin());
            
                recordRemainTime = recordRange;
            }

            float average = 0.0f;
            for (const float& v : m_fpsHistory)
                average += v;
            average /= (float)m_fpsHistory.size();

            std::string str = std::format("Average : {}", (int)average);
            ImGui::PlotLines("###FpsGraph", m_fpsHistory.data(), sizeof(m_fpsHistory.data()), m_fpsHistory.size(), str.c_str(), 1, 1000, {0.f,80.f});
            ImGui::Text(std::format("FPS : {}", IO->Framerate).c_str());
        }

        std::string name = std::filesystem::path(SceneManager::GetInstance()->m_currentScene->GetName()).filename().string();
        //ImCherry::CenteredText(name);
        ImCherryInternal::BeginCherryHeader(false);
        ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Framed);
        ImGui::TreePop();
        ImCherryInternal::EndCherryHeader();
        Cell* selectedCell = m_cellSystemDisplayer->GetSelectedCell();

        if (!selectedCell)
        {
            ImGui::Text("Please Select a Cell");
            ImGui::End();
            ImGui::PopStyleVar();
            return;
        }

        ImGui::Text(std::format("Cell : {}", selectedCell->GetName().c_str()).c_str());
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 12,2 });
        ImGui::BeginChild("###HIERARCHY", {ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y * .9f});
        ImGui::Dummy({ 1.f,8.f });
        for (auto& node : m_nodes)
        {
            if (!node.m_isRoot && node.m_entityTransform)
                continue;

            if (RenderEntity(node))
                break;
        }

        HandleShortcuts();
        
        ImGui::EndChild();
        ImGui::PopStyleVar();
        if (selectedCell->m_skyRenderer)
        {
            auto& metapack = selectedCell->m_skyRenderer->GetMetapack();
            for (auto& [metaname, metadata] : metapack)
            {
                std::string defaultVal;
                std::string* valPtr = &defaultVal;
                metadata->Get((void**)&valPtr);

                if (metadata->m_identifier.find("dropzone") != std::string::npos)
                {
                    bool disabled = metadata->m_identifier.find("off") != std::string::npos;
                    std::string name = std::filesystem::path(defaultVal).filename().string();

                    void* data = nullptr;
                    ImCherry::DropzoneMenu(metaname, name, "NODE", &data, disabled);

                    if (data) 
                    {
                        const char* cdata = (const char*) data;
                        std::string strData = cdata;
                        metadata->Set(&strData);
                    }

                }
            }
            
        }
    }


    ImGui::End();
    ImGui::PopStyleVar();

    {
        if (m_isRenaming)
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
                m_isRenaming = false;
                SceneManager::SetHierarchyDirty(true);
            }
            ImGui::SetItemDefaultFocus();
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0))) { m_isRenaming = false;  ImGui::CloseCurrentPopup(); }
            ImGui::EndPopup();
        }
    }
}
 
bool HierarchyDisplayer::RenderEntity(HierarchyNode& node)
{
    //References
    EntitySelector* selector = &m_manager->m_entitySelector;
    Transform* entityTransform = node.m_entityTransform;
    Entity* entity = node.m_entity;

    // TreeNode Flags
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
    if (selector->Contains(node.m_entity))                                     flags |= ImGuiTreeNodeFlags_Selected;
    if (!entityTransform || entityTransform->GetChildren()->size() <= 0)       flags |= ImGuiTreeNodeFlags_Leaf;

    // Tree node display
    bool opened = ImGui::TreeNodeEx((void*)(intptr_t)entity->GetUUID(), flags, node.m_name.c_str());

    // Drag Source
    if (ImGui::BeginDragDropSource())
    {
        ImGui::SetDragDropPayloadNoCopy("HIERARCHY_DROP", entity, sizeof(Entity), ImGuiCond_Once);
        ImGui::Text(node.m_name.c_str());
        ImGui::EndDragDropSource();
        m_isDragging = true;
    }

    // Selector click behaviour
    // If left click is released and the node is hovered and there is no drag 
    if ((InputManager::GetKeyUp(Keycode::LEFT_CLICK) || InputManager::GetKeyDown(Keycode::RIGHT_CLICK))
        && ImGui::IsItemHovered() && !m_isDragging)
    {
        if (InputManager::GetKey(Keycode::LEFT_CONTROL))    // add node to current selection
        {
            if (!selector->Contains(entity))
                selector->Add(entity);
            else
                selector->Remove(entity);
        }
        else if (InputManager::GetKey(Keycode::LEFT_SHIFT)) // add node range to current selection
        {
            selector->SetEndRange();
            selector->SetStartRange(entity);
            selector->ApplyRange();
        }
        else // set node as selection
        {
            m_manager->FocusEntity(entity);
        }

        selector->SetStartRange(entity);
    }
    

    // If entity dropped on an other that got transform and so child system
    if (entityTransform && ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_DROP"))
        {
            m_draggedEntity = static_cast<Entity*>(payload->Data);
            if (Transform* droppedTransform = m_draggedEntity->GetBehaviour<Transform>()) 
            {
                droppedTransform->SetParent(entityTransform, true, true, true);
                m_manager->m_entitySelector.Clear();
                EmptyDrop();
                if (opened) ImGui::TreePop();
                return true;
            }
        }

        ImGui::EndDragDropTarget();
    }
    // If entity is dropped on window, but out of entities
    else if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && ImGui::GetCurrentContext()->DragDropActive && ImGui::IsWindowHovered())
    {
        ImGuiPayload* payload = &ImGui::GetCurrentContext()->DragDropPayload;
        if (payload->IsDataType("HIERARCHY_DROP"))
            m_draggedEntity = static_cast<Entity*>(payload->Data);

    }
    else if (m_draggedEntity) // draggedEntity drop at frame + 1
    {
        Transform* draggedTransform = m_draggedEntity->GetBehaviour<Transform>();
        if (draggedTransform && draggedTransform == entityTransform)
        {
            if (m_isDragging)
            {
                m_isDragging = false;
                m_draggedEntity = entity;
            }
            else
            {
                m_draggedEntity->GetBehaviour<Transform>()->SetParent(nullptr, true, true, true);
                m_manager->FocusEntity(m_draggedEntity);
                m_draggedEntity = nullptr;
                if (opened) ImGui::TreePop();
                return true;
            }
        }
    }

    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && ImGui::GetCurrentContext()->DragDropActive && !ImGui::IsWindowHovered()) 
    {
        EmptyDrop();
    }

    // Recursive call
    if (opened && entityTransform)
    {
        for (auto& child : node.m_childrens)
            if (RenderEntity(child)) { ImGui::TreePop(); return true; }
    }

    // Pop if opened node
    if (opened)
        ImGui::TreePop();

    return false;
}

void HierarchyDisplayer::ContextCallback()
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

                        PortalComponent* portal_0 = entity_0->AddBehaviour<PortalComponent>();
                        PortalComponent* portal_1 = entity_1->AddBehaviour<PortalComponent>();

                        entity_0->Initialize();
                        entity_1->Initialize();

                        scene->AddEntity(entity_0);
                        m_manager->FocusEntity(entity_0);

                        scene->AddEntity(entity_1);
                        m_manager->FocusEntity(entity_1);

                        portal_0->SetLinkedPortal(portal_1);
                        //portal_1->SetLinkedPortal(portal_0);
                    }
                }

                ImGui::EndMenu();
            }

            if (ImGui::MenuItem("Camera")) 
            {
                if (Cell* cell = m_cellSystemDisplayer->GetSelectedCell())
                {
                    newEntity = new Entity("Camera", cell);
                    newEntity->AddBehaviour<CameraComponent>();
                    newEntity->AddBehaviour<AudioListener>();
                    newEntity->Initialize();
                }
            }

            if (ImGui::MenuItem("Particle System")) {}
            if (ImGui::MenuItem("Audio Source"))
            {
                if (Cell* cell = m_cellSystemDisplayer->GetSelectedCell())
                {
                    newEntity = new Entity("Audio Source", cell);
                    newEntity->AddBehaviour<AudioEmitter>();
                    newEntity->Initialize();
                }
            }

            if (ImGui::MenuItem("Light"))
            {
                if (Cell* cell = m_cellSystemDisplayer->GetSelectedCell())
                {
                    Scene* scene = SceneManager::GetInstance()->m_currentScene.get();

                    newEntity = new Entity("Light", cell);
                    newEntity->AddBehaviour<LightComponent>();
                    newEntity->Initialize();
                }
            }

            if (ImGui::BeginMenu("Shapes"))
            {
                //Entity* newEntity = nullptr;

                //std::shared_ptr<Model> newModel = nullptr;
                if (Cell* cell = m_cellSystemDisplayer->GetSelectedCell())
                {
                    ShapeRenderer* shape = nullptr;
                    if (ImGui::MenuItem("Cube"))
                    {
                        newEntity = new Entity("Cube", cell);
                        Transform* tranform = newEntity->AddBehaviour<Transform>();
                        shape = newEntity->AddBehaviour<ShapeRenderer>();
                        shape->m_transform = tranform;
                        shape->SetType((int)EMeshShape::CUBE);
                        newEntity->AddBehaviour<BoxCollider>();
                    }

                    if (ImGui::MenuItem("Sphere"))
                    {
                        newEntity = new Entity("Sphere", cell);
                        Transform* tranform = newEntity->AddBehaviour<Transform>();
                        shape = newEntity->AddBehaviour<ShapeRenderer>();
                        shape->m_transform = tranform;
                        shape->SetType((int)EMeshShape::SPHERE);
                        newEntity->AddBehaviour<SphereCollider>();
                    }
                    if (ImGui::MenuItem("Cylinder"))
                    {
                        newEntity = new Entity("Cylinder", cell);
                        Transform* tranform = newEntity->AddBehaviour<Transform>();
                        shape = newEntity->AddBehaviour<ShapeRenderer>();
                        shape->m_transform = tranform;
                        shape->SetType((int)EMeshShape::CYLINDER);
                        newEntity->AddBehaviour<CapsuleCollider>();
                    }
                    if (ImGui::MenuItem("Plane"))
                    {
                        newEntity = new Entity("Plane", cell);
                        Transform* tranform = newEntity->AddBehaviour<Transform>();
                        shape = newEntity->AddBehaviour<ShapeRenderer>();
                        shape->m_transform = tranform;
                        shape->SetType((int)EMeshShape::QUAD);
                        newEntity->AddBehaviour<BoxCollider>();
                    }

                    if (newEntity)
                    {
                        std::shared_ptr<Material> material = ResourceManager::GetInstance()->AddResourceRef<Material>("CC_Mat");
                        shape->SetMaterial(material);
                        newEntity->Initialize();
                    }
                }

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

            if (ImGui::MenuItem("Rename")) { m_isRenaming = true; }
            if (ImGui::MenuItem("Delete"))
            {
                for (int i = static_cast<int>(m_manager->m_entitySelector.m_entities.size()) - 1; i >= 0; i--)
                {
                    Entity* entity = m_manager->m_entitySelector.m_entities[i];
                     
                    entity->m_OnUnselected.Invoke();
                    
                    if (m_manager->m_entitySelector.Contains(entity))
                        m_manager->m_entitySelector.Remove(entity);

                    SceneManager::GetInstance()->m_currentScene->RemoveEntity(entity);


                }
                SceneManager::SetHierarchyDirty();
            }

            if (ImGui::MenuItem("Copy")) 
            {
                for (auto& entity : m_manager->m_entitySelector.m_entities)
                {
                    SceneManager::GetInstance()->m_currentScene->CopyEntity(entity);
                }
                SceneManager::SetHierarchyDirty();
            }

            ImGui::Separator();
        }
        if (ImGui::MenuItem("Paste")) {}

        ImGui::EndPopup();
    
}

void HierarchyDisplayer::Refresh()
{
    m_nodes.clear();

    Cell* selectedCell = m_cellSystemDisplayer->GetSelectedCell();
    if (!selectedCell) 
    {
        SceneManager::SetHierarchyDirty(false);
        return;
    }
       

    auto entities = selectedCell->GetEntities();

    for (auto& entity : entities)
    {
        HierarchyNode node = { entity };
        node.m_entityTransform = entity->GetBehaviour<Transform>();
        node.m_isRoot = node.m_entityTransform ? node.m_entityTransform->IsRoot() : false;
        node.m_name = entity->GetName();
        m_nodes.push_back(node);
    }

    for (auto& node : m_nodes) 
    {
        if (!node.m_entityTransform)
            continue;
        
        auto children = node.m_entityTransform->GetChildren();
        for (const auto& child : *children)
        {
            for (auto& itNode : m_nodes) 
            {
                if (itNode.m_entityTransform == child) 
                {
                    node.m_childrens.push_back(itNode);
                    continue;
                }
            }
        }
    }

    SceneManager::SetHierarchyDirty(false);
}
