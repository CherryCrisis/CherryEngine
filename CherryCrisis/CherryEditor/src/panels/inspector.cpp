#include "panels/inspector.hpp"

#include <algorithm>
#include <string>
#include <typeinfo>

#include <imgui.h>

#include "cherry_header.hpp"

#include "audio_emitter.hpp"
#include "audio_listener.hpp"
#include "basic_renderpass.hpp"
#include "bool3.hpp"
#include "box_collider.hpp"
#include "camera_component.hpp"
#include "portal_teleporter_component.hpp"
#include "capsule_collider.hpp"
#include "character_controller.hpp"
#include "core/editor_manager.hpp"
#include "core/imcherry.hpp"
#include "csscripting_system.hpp"
#include "entity.hpp"
#include "light_component.hpp"
#include "material.hpp"
#include "panels/asset_settings.hpp"
#include "portal_component.hpp"
#include "rigidbody.hpp"
#include "shape_renderer.hpp"
#include "scripted_behaviour.hpp"
#include "sphere_collider.hpp"
#include "transform.hpp"
#include "light.hpp"
#include "utils.hpp"


#define IMGUI_LEFT_LABEL(func, label, ...) (ImGui::TextUnformatted(label), ImGui::SameLine(), func("##" label, __VA_ARGS__))

Inspector::Inspector(bool spawnOpened, EditorManager* manager, AssetSettingsDisplayer* assetSettingsDisplayer) 
    : Panel(spawnOpened), m_manager(manager), m_assetSettingsDisplayer(assetSettingsDisplayer)
{
    PopulateComponentList();
}

void Inspector::PopulateComponentList() 
{
    m_componentsNames.clear();

    //                            Reflection Type           |       Display Name
    m_componentsNames.push_back({ "Transform",                      "Transform"});
    m_componentsNames.push_back({ "CameraComponent",                "Camera Component" });
    m_componentsNames.push_back({ "LightComponent",                 "Light Component" });
    m_componentsNames.push_back({ "Rigidbody",                      "Rigidbody" });
    m_componentsNames.push_back({ "AudioEmitter",                   "Audio Emitter" });
    m_componentsNames.push_back({ "AudioListener",                  "Audio Listener" });
    m_componentsNames.push_back({ "SphereCollider",                 "Sphere Collider" });
    m_componentsNames.push_back({ "BoxCollider",                    "Box Collider" });
    m_componentsNames.push_back({ "CapsuleCollider",                "Capsule Collider" });
    m_componentsNames.push_back({ "CharacterController",            "Character Controller" });
    m_componentsNames.push_back({ "PortalComponent",                "Portal Component" });
    m_componentsNames.push_back({ "PortalTeleporterComponent",      "Portal Teleporter" });

    for (const std::string& name : CsScriptingSystem::GetInstance()->classesName)
    {
        m_componentsNames.push_back({"ScriptedBehaviour", name});
    }
}

void Inspector::DropzoneCallback(const std::string& path) 
{
    if (path.find(".ccmat") != std::string::npos) 
    {
        std::shared_ptr<Material> mat = ResourceManager::GetInstance()->GetResource<Material>(path.c_str());
        if (m_assetSettingsDisplayer)
            m_assetSettingsDisplayer->SetAssetSettings(new MaterialSettings(mat));
    }
}

void Inspector::RenderAddComponentList()
{
    ImVec2 center = { InputManager::GetMousePos().x, InputManager::GetMousePos().y };
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing);
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize;

    if (ImGui::BeginPopupModal("Add Component", NULL, flags))
    {
        static char search[32] = "";

        ImCherry::SetKeyboardFocus();
        ImGui::InputText("##", search, IM_ARRAYSIZE(search));

        std::vector<Behaviour*> addedBehaviours;
        ImGui::BeginChild("CompList", { ImGui::GetContentRegionAvail().x, 150.f }, false);

        for (const auto& comp : m_componentsNames) 
        { 
            if (strlen(search) > 0)
                if (String::ToLower(comp.displayName).find(String::ToLower(search)) == std::string::npos)
                    continue;
            
            if (ImGui::MenuItem(comp.displayName.c_str()))
            {
                for (Entity* entity : m_manager->m_entitySelector.m_entities) 
                {
                    if (comp.type == "Transform" && entity->GetBehaviour<Transform>())
                    {
                        EditorManager::SendNotification("There is already a Transform !", ENotifType::Warning);
                        continue;
                    }

                    Behaviour* behaviour = Serializer::CreateBehaviour(comp.type, {}, false);
                    addedBehaviours.push_back(behaviour);

                    if (comp.type == "ScriptedBehaviour")
                        static_cast<ScriptedBehaviour*>(behaviour)->SetScriptClass(comp.displayName);
                    
                    behaviour->SetHostPtr(entity);
                }

                ImGui::CloseCurrentPopup();
            }
        }

        if (addedBehaviours.size() > 0)
        {
            for (Entity* entity : m_manager->m_entitySelector.m_entities)
                entity->Initialize();
        }
        ImGui::EndChild();

        if (ImGui::Button("Cancel", ImVec2(ImGui::GetContentRegionAvail().x, 0))) { ImGui::CloseCurrentPopup(); }
        ImGui::EndPopup();
    }
}

void Inspector::InspectComponents(Entity* entity, int id)
{
    std::vector<Behaviour*> behaviours = entity->GetAllBehaviours();

    for (Behaviour* behaviour : behaviours)
    {
        ImGui::PushID(static_cast<int>(behaviour->GetUUID()));

        ImCherryInternal::BeginCherryHeader();
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen;

        ////------- Calculate values to dislpay metadata background -------
        auto dl = ImGui::GetWindowDrawList();
        dl->AddRectFilled({ 1,1 }, { 1,1 }, IM_COL32(21, 21, 26, 255));

        ImDrawVert& vert =  dl->VtxBuffer[dl->VtxBuffer.size() - 1];
        ImDrawVert& vert1 = dl->VtxBuffer[dl->VtxBuffer.size() - 2];
        ImDrawVert& vert2 = dl->VtxBuffer[dl->VtxBuffer.size() - 3];
        ImDrawVert& vert3 = dl->VtxBuffer[dl->VtxBuffer.size() - 4];

        ImGui::BeginGroup();
        bool opened = ImGui::TreeNodeEx(behaviour->TypeName().c_str(), flags);
        float xmin = ImGui::GetItemRectMin().x; float xmax = ImGui::GetItemRectMax().x;
        ImVec2 headerSize = ImGui::GetItemRectSize();
        ////---------------------------------------------------------------
 
        ImCherryInternal::EndCherryHeader();
        
        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("Delete"))
            {
                if (entity->RemoveBehaviour(behaviour))
                {
                    ImGui::EndPopup(); 
                    if (opened) 
                    {
                        ImGui::TreePop();
                        ImGui::EndGroup();
                    }
                    ImGui::PopID();
                    return;
                }
            }
            if (ImGui::MenuItem("Copy state"))
            {
                m_copiedBehaviour = behaviour;
            }
            if (!m_copiedBehaviour) ImGui::BeginDisabled();
            if (ImGui::MenuItem("Paste state"))
            {
                if (m_copiedBehaviour) 
                {
                    if (!behaviour->TypeName().compare(m_copiedBehaviour->TypeName())) 
                    {
                        behaviour->Copy(m_copiedBehaviour);
                    }
                }
            }
            if (!m_copiedBehaviour) ImGui::EndDisabled();

            if (!m_copiedBehaviour) ImGui::BeginDisabled();
            if (ImGui::MenuItem("Paste as new"))
            {
                if (m_copiedBehaviour)
                {
                    std::string name = String::ExtractLastValue(typeid(*m_copiedBehaviour).name(), ' ');
                    Behaviour* behaviourCopy = Serializer::CreateBehaviour(name, {},false);

                    if (behaviourCopy)
                        behaviourCopy->SetHostPtr(entity);

                    behaviourCopy->Copy(behaviour);
                }
            }
            if (!m_copiedBehaviour) ImGui::EndDisabled();

            ImGui::EndPopup();
        }

        if (opened)
        {
            auto& metapack = behaviour->GetMetapack();
            for (auto& [metaname, metadata] : metapack)
            { 
                if (!metadata->m_isShownOnInspector)
                    continue;

                const std::type_index& type = metadata->GetType();

                if (type == typeid(CCMaths::Vector3))
                {
                    CCMaths::Vector3 defaultVal;
                    CCMaths::Vector3* valPtr = &defaultVal;
                    metadata->Get((void**)&valPtr);
                    if (valPtr && ImCherry::ColoredDragFloat3(metaname, valPtr->data, metadata->m_identifier == "toDegree"))
                        metadata->Set(valPtr);
                    continue;
                }

                if (type == typeid(Bool3))
                {
                    Bool3 defaultVal;
                    Bool3* valPtr = &defaultVal;
                    metadata->Get((void**)&valPtr);

                    if (valPtr)
                    {
                        if (ImCherry::CheckboxBool3(metaname, valPtr->data))
                            metadata->Set(valPtr);
                    }                   
                    continue;
                }

                if (type == typeid(bool))
                {
                    bool defaultVal;
                    bool* valPtr = &defaultVal;
                    metadata->Get((void**)&valPtr);
                    if (valPtr && ImCherry::Checkbox(metaname, valPtr))
                        metadata->Set(valPtr);

                    continue;
                }

                if (type == typeid(int))
                {
                    int defaultVal;
                    int* valPtr = &defaultVal;
                    metadata->Get((void**)&valPtr);
                    if (valPtr && ImGui::DragInt(metaname.c_str(), valPtr, 0.5f))
                        metadata->Set(valPtr);

                    continue;
                }
                if (type == typeid(ELightType))
                {
                    ELightType defaultVal = ELightType::OFF;
                    ELightType* valPtr = &defaultVal;
                    metadata->Get((void**)&valPtr);
                    if (!valPtr)
                        continue;

                    const char* items[] = { "Off", "Point", "Directional" };
                    const char* currentItem = items[(unsigned int)*valPtr];

                    if (ImGui::BeginCombo(metaname.c_str(), currentItem))
                    {
                        for (int n = 0; n < IM_ARRAYSIZE(items); n++)
                        {
                            bool is_selected = currentItem == items[n];
                            if (ImGui::Selectable(items[n], is_selected))
                            {
                                *valPtr = (ELightType)n;
                                metadata->Set(valPtr);
                            }
                        }

                        ImGui::EndCombo();
                    }

                    continue;
                }
                if (type == typeid(float))
                {
                    float defaultVal;
                    float* valPtr = &defaultVal;
                    metadata->Get((void**)&valPtr);
                    if (valPtr && ImCherry::DragFloat(metaname, valPtr, 0.5f, 0.0f, 0.0f, *valPtr >= 1.0e+5 ? "%e" : "%.3f"))
                        metadata->Set(valPtr);

                    continue;
                }

                if (type == typeid(std::string))
                {
                    std::string defaultVal;
                    std::string* valPtr = &defaultVal;
                    metadata->Get((void**)&valPtr);

                    if (metadata->m_identifier.find("dropzone") != std::string::npos)
                    {
                        bool disabled = metadata->m_identifier.find("off") != std::string::npos;
                        std::string name = std::filesystem::path(defaultVal).filename().string();
                        
                        if (ImCherry::DropzoneMenu(metaname, name, disabled)) 
                            DropzoneCallback(defaultVal);

                        if (ImGui::BeginDragDropTarget() && !disabled)
                        {
                            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("NODE"))
                            {
                                const char* data = (const char*)payload->Data;
                                std::string strData = data;
                                metadata->Set(&strData);
                            }
                            ImGui::EndDragDropTarget();
                        }
                    }
                    else 
                    {
                        if (valPtr && ImGui::InputText(metaname.c_str(), &(*valPtr)[0], valPtr->size() + 1))
                            metadata->Set(valPtr);
                    }

                    continue;
                }

                if (type == typeid(Entity*))
                {
                    Entity* defaultVal = nullptr;
                    Entity** valPtr = &defaultVal;
                    metadata->Get((void**)&valPtr);

                    ImGui::Text("%s", metaname.c_str());

                    if (ImGui::BeginDragDropTarget())
                    {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_DROP"))
                        {
                            Entity* m_draggedObject = (Entity*)payload->Data;
                            metadata->Set(&m_draggedObject);
                        }

                        ImGui::EndDragDropTarget();
                    }

                    continue;
                }

                if (type == typeid(Behaviour*))
                {
                    Behaviour* defaultVal = nullptr;
                    Behaviour** valPtr = &defaultVal;
                    metadata->Get((void**)&valPtr);

                    std::string fieldContent = metadata->m_identifier + " (" + (*valPtr ? (*valPtr)->GetHost().GetName() : "null") + ')';
                    
                    ImCherry::Dropzone(metaname, fieldContent);

                    if (ImGui::BeginDragDropTarget())
                    {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_DROP"))
                        {
                            Entity* draggedObject = (Entity*)payload->Data;
                            *valPtr = draggedObject->GetBehaviour(metadata->m_identifier);

                            if (!*valPtr)
                            {
                                std::string dragError = draggedObject->GetName() + " does not have any " + metadata->m_identifier + " component!";
                                EditorManager::SendNotification(dragError.c_str(), ENotifType::Error, 2.f);
                            }
                            metadata->Set(valPtr);
                        }
                        ImGui::EndDragDropTarget();
                    }
                    continue;
                }
            }
            ImGui::TreePop();
        }
        ImGui::EndGroup();

        if (opened) // If opened calculate and draw component background
        {
            float ymin = ImGui::GetItemRectMin().y; float ymax = ImGui::GetItemRectMax().y;
            ymin += headerSize.y / 2.f;
            ymax += ImGui::GetStyle().FramePadding.y;
            vert.pos = { xmin, ymin };
            vert1.pos = { xmax, ymin };
            vert2.pos = { xmax, ymax };
            vert3.pos = { xmin, ymax };
        }

        ImGui::PopID();
    }  
}

void InspectMultiComponents(std::vector<Entity*> entities)
{
    bool areDifferents = false;

    Transform* first = nullptr;

    for (Entity* entity : entities) 
    {
        Transform* found = nullptr;
        entity->TryGetBehaviour<Transform>(found);
        if (found) 
        {
            if (!first) first = found;
            else 
            {
                areDifferents |= first->GetPosition() != found->GetPosition();
                areDifferents |= first->GetRotation() != found->GetRotation();
                areDifferents |= first->GetScale()    != found->GetScale();
            }

        }
    }


   
    if (areDifferents)
        ImGui::Text("Transforms are differents !");
    else
    {
        /*
        CCMaths::Vector3 position = first->GetPosition();
        CCMaths::Vector3 rotation = first->GetRotation();
        CCMaths::Vector3 scale = first->GetScale();

        ImVec4 color1 = { 100.f / 255.f, 10.f / 255.f, 10.f / 255.f, 1.f };
        ImVec4 color2 = { 18.f / 255.f, 120.f / 255.f, 4.f / 255.f, 1.f };
        ImVec4 color3 = { 12.f / 255.f, 50.f / 255.f, 170.f / 255.f, 1.f };

        if (ImCherry::ColoredDragFloat3("position", position.data, color1, color2, color3, 0.5f))
        {
            for (Entity* entity : entities)
            {
                Transform* found = nullptr;
                entity->TryGetBehaviour<Transform>(found);
                if (found)
                    found->GetProperties()["position"]->Set(&position);
            }
        }
        if (ImCherry::ColoredDragFloat3("rotation", rotation.data, color1, color2, color3, 0.5f))
        {
            for (Entity* entity : entities)
            {
                Transform* found = nullptr;
                entity->TryGetBehaviour<Transform>(found);
                if (found)
                    found->GetProperties()["rotation"]->Set(&rotation);
            }
        }
        if (ImCherry::ColoredDragFloat3("scale", scale.data, color1, color2, color3, 0.5f))
        {
            for (Entity* entity : entities)
            {
                Transform* found = nullptr;
                entity->TryGetBehaviour<Transform>(found);
                if (found)
                    found->GetProperties()["scale"]->Set(&scale);
            }
        }*/
        ImGui::Text("Transforms are the same !");
    }
}

void Inspector::Render() 
{
    if (!m_isOpened)
        return;

    if (ImGui::Begin("Inspector", &m_isOpened))
    {
        EntitySelector& selector = m_manager->m_entitySelector;
        if (!selector.IsEmpty()) 
        {
            if (selector.Count() == 1)
                InspectComponents(selector.m_entities[0], 0);
            else
                InspectMultiComponents(selector.m_entities);

            ImVec2 buttonSize = { ImGui::GetContentRegionAvail().x, 0 };
            if (ImCherry::ColoredButton("Add Component", { 0.166f, 0.166f, 0.221f, 1.000f }, buttonSize))
                ImGui::OpenPopup("Add Component");       
        }

        RenderAddComponentList();
    }
    ImGui::End();
}
