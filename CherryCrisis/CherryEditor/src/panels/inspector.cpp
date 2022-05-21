
#include "panels/inspector.hpp"

#include <imgui.h>
#include <string>
#include <typeinfo>
#include <algorithm>

#include "cherry_header.hpp"
#include "core/editor_manager.hpp"

#include "audio_emitter.hpp"
#include "audio_listener.hpp"
#include "basic_renderpass.hpp"
#include "bool3.hpp"
#include "box_collider.hpp"
#include "camera_component.hpp"
#include "capsule_collider.hpp"
#include "character_controller.hpp"
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
#include "utils.hpp"

#define IMGUI_LEFT_LABEL(func, label, ...) (ImGui::TextUnformatted(label), ImGui::SameLine(), func("##" label, __VA_ARGS__))


Inspector::Inspector(bool spawnOpened, EditorManager* manager, AssetSettingsDisplayer* assetSettingsDisplayer) 
    : Panel(spawnOpened), m_manager(manager), m_assetSettingsDisplayer(assetSettingsDisplayer)
{
    
}

std::string ExtractValue(std::string str, const char key = ':')
{
    return str.substr(str.find_first_of(key) + 1);
}

void Inspector::InspectComponents(Entity* entity, int id)
{
    std::vector<Behaviour*> behaviours = entity->GetAllBehaviours();

    ModelRenderer* renderer = nullptr;
    ShapeRenderer* shape = nullptr;
    AudioEmitter* emitter = nullptr;
    for (Behaviour* behaviour : behaviours)
    {
        ImGui::PushID(static_cast<int>(behaviour->GetUUID()));
        std::string bname = typeid(*behaviour).name();
        bname = ExtractValue(bname, ' ');

        if (ImGui::BeginPopupContextItem("context"))
        {
            if (ImGui::MenuItem("Delete")) 
            {
                if (entity->RemoveBehaviour(behaviour)) 
                {
                    ImGui::EndPopup(); ImGui::PopID();
                    break;
                }

            }
            if (ImGui::MenuItem("Copy")) {}
            if (ImGui::MenuItem("Paste")) {}
            ImGui::EndPopup();
        }
        bool opened = false;
        ScriptedBehaviour* b = (ScriptedBehaviour*)behaviour;
        if (bname == "ScriptedBehaviour")
            opened = ImGui::TreeNode(b->GetScriptPath().c_str());
        else
            opened = ImGui::TreeNode(bname.c_str());
        if (bname == "ModelRenderer")
            renderer = (ModelRenderer*)behaviour;
        if (bname == "AudioEmitter")
            emitter = (AudioEmitter*)behaviour;
        if (bname == "ShapeRenderer")
            shape = (ShapeRenderer*)behaviour;
        // check if right clicked
        if (InputManager::GetKeyDown(Keycode::RIGHT_CLICK) && ImGui::IsItemHovered())
        {
            ImGui::OpenPopup("context");
        }

        ImVec4 color1 = { 100.f / 255.f, 10.f / 255.f, 10.f / 255.f, 1.f };
        ImVec4 color2 = { 18.f / 255.f, 120.f / 255.f, 4.f / 255.f, 1.f };
        ImVec4 color3 = { 12.f / 255.f, 50.f / 255.f, 170.f / 255.f, 1.f };

        if (opened)
        {
            auto& metapack = behaviour->GetMetapack();
            for (auto& [metaname, metadata] : metapack)
            {
                const std::type_index& type = metadata->GetType();

                if (type == typeid(CCMaths::Vector3))
                {
                    CCMaths::Vector3 defaultVal;
                    CCMaths::Vector3* valPtr = &defaultVal;
                    metadata->Get((void**)&valPtr);
                    if (valPtr && ImCherry::ColoredDragFloat3(metaname.c_str(), valPtr->data, color1, color2, color3, 0.5f))
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
                        bool hasChecked = false;
                        hasChecked |= ImGui::Checkbox(metaname.c_str(), &valPtr->x); ImGui::SameLine();
                        hasChecked |= ImGui::Checkbox(metaname.c_str(), &valPtr->y); ImGui::SameLine();
                        hasChecked |= ImGui::Checkbox(metaname.c_str(), &valPtr->z);

                        if (hasChecked)
                            metadata->Set(valPtr);
                    }
                    
                    continue;
                }

                if (type == typeid(bool))
                {
                    bool defaultVal;
                    bool* valPtr = &defaultVal;
                    metadata->Get((void**)&valPtr);
                    if (valPtr && ImGui::Checkbox(metaname.c_str(), valPtr))
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
                if (type == typeid(float))
                {
                    float defaultVal;
                    float* valPtr = &defaultVal;
                    metadata->Get((void**)&valPtr);
                    if (valPtr && ImGui::DragFloat(metaname.c_str(), valPtr, 0.5f, 0.0f, 0.0f, *valPtr >= 1.0e+5 ? "%e" : "%.3f"))
                        metadata->Set(valPtr);

                    continue;
                }

                if (type == typeid(std::string))
                {
                    std::string defaultVal;
                    std::string* valPtr = &defaultVal;
                    metadata->Get((void**)&valPtr);
                    if(valPtr && ImGui::InputText(metaname.c_str(), &(*valPtr)[0], valPtr->size() + 1))
                        metadata->Set(valPtr);

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
                    ImGui::BeginDisabled();
                    ImGui::InputText(metaname.c_str(), (char*)fieldContent.c_str(), fieldContent.size());
                    ImGui::EndDisabled();

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
        ImGui::Separator();
        ImGui::PopID();
    }
    
    //Inspect Material
    if (renderer && renderer->m_mesh)
    {
        std::shared_ptr<Material> mat = renderer->m_material;
        std::shared_ptr<Material> newMat = InspectMaterial(mat);

        if (newMat)
        {
            renderer->SetMaterial(newMat);
        }
    }

    //Inspect Material
    if (shape && shape->m_mesh)
    {
        std::shared_ptr<Material> mat = shape->m_material;
        std::shared_ptr<Material> newMat = InspectMaterial(mat);

        if (newMat)
        {
            shape->SetMaterial(newMat);
        }
    }
}

std::shared_ptr<Material> Inspector::InspectMaterial(std::shared_ptr<Material> material)
{
    if (ImGui::TreeNode("Material"))
    {
        if (material)
        {
            if (ImGui::Button(material->GetFilesystemPath()->filename().string().c_str()))
            {
                if (m_assetSettingsDisplayer)
                    m_assetSettingsDisplayer->SetAssetSettings(new MaterialSettings(material));
            }
        }
        else
            ImGui::Button("Empty");

        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("NODE"))
            {
                const char* materialPath = (const char*)payload->Data;
                std::string extension = String::ExtractValue(materialPath, '.');

                if (!matExtensions.compare("." + extension))
                {
                    std::shared_ptr<Material> newMaterial = ResourceManager::GetInstance()->GetResource<Material>(materialPath);

                    if (newMaterial)
                    {
                        ImGui::EndDragDropTarget();
                        ImGui::TreePop();

                        return newMaterial;
                    }
                }
            }
            ImGui::EndDragDropTarget();
        }

        ImGui::TreePop();
    }

    return nullptr;
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

            if (ImGui::Button("Add Component", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
                ImGui::OpenPopup("Add Component");
        }

        ImVec2 center = { InputManager::GetMousePos().x, InputManager::GetMousePos().y };
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing);
        if (ImGui::BeginPopupModal("Add Component", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            static char search[32] = "";
            ImGui::InputText("##", search, IM_ARRAYSIZE(search));

            std::vector<Behaviour*> addBehaviours;

            // TODO: Replace with list of available components
            if (ImGui::MenuItem("Transform")) 
            { 
                for (Entity* entity : m_manager->m_entitySelector.m_entities) 
                    addBehaviours.push_back(entity->AddBehaviour<Transform>());
            }
            if (ImGui::MenuItem("Camera"))      
            {
                for (Entity* entity : m_manager->m_entitySelector.m_entities)
                    addBehaviours.push_back(entity->AddBehaviour<CameraComponent>());
            }
            if (ImGui::MenuItem("Light"))
            {
                for (Entity* entity : m_manager->m_entitySelector.m_entities)
                    addBehaviours.push_back(entity->AddBehaviour<LightComponent>());
            }
            if (ImGui::MenuItem("Rigidbody"))     
            {
                for (Entity* entity : m_manager->m_entitySelector.m_entities)
                    addBehaviours.push_back(entity->AddBehaviour<Rigidbody>());
            }
            if (ImGui::MenuItem("Audio Listener"))
            {
                for (Entity* entity : m_manager->m_entitySelector.m_entities) 
                {
                    AudioListener* listener =  entity->AddBehaviour<AudioListener>();
                    listener->Initialize();
                }
            }
            if (ImGui::MenuItem("Audio Emitter"))
            {
                for (Entity* entity : m_manager->m_entitySelector.m_entities) 
                {
                    AudioEmitter* emitter = entity->AddBehaviour<AudioEmitter>();
                    emitter->Initialize();
                }
            }
            if (ImGui::MenuItem("Box Collider"))      
            {
                for (Entity* entity : m_manager->m_entitySelector.m_entities)
                    addBehaviours.push_back(entity->AddBehaviour<BoxCollider>());
            }
            if (ImGui::MenuItem("Sphere Collider"))
            {
                for (Entity* entity : m_manager->m_entitySelector.m_entities)
                    addBehaviours.push_back(entity->AddBehaviour<SphereCollider>());
            }
            if (ImGui::MenuItem("Capsule Collider")) 
            {
                for (Entity* entity : m_manager->m_entitySelector.m_entities)
                    addBehaviours.push_back(entity->AddBehaviour<CapsuleCollider>());
            }
            if (ImGui::MenuItem("Character Controller"))
            {
                for (Entity* entity : m_manager->m_entitySelector.m_entities)
                    addBehaviours.push_back(entity->AddBehaviour<CharacterController>());
            }
            if (ImGui::MenuItem("Portal Component"))
            {
                for (Entity* entity : m_manager->m_entitySelector.m_entities)
                    addBehaviours.push_back(entity->AddBehaviour<PortalComponent>());
            }
            for (const std::string& name : CsScriptingSystem::GetInstance()->classesName) 
            {
                if (ImGui::MenuItem(name.c_str()))
                {
                    for (Entity* entity : m_manager->m_entitySelector.m_entities) 
                    {
                        ScriptedBehaviour* behaviour = entity->AddBehaviour<ScriptedBehaviour>(name);
                        behaviour->SetScriptClass(name);
                    }
                }
            }  

            if (addBehaviours.size() > 0)
            {
                for (Entity* entity : m_manager->m_entitySelector.m_entities)
                    entity->Initialize();
            }

            //---------------------------------------------------
            ImGui::SetItemDefaultFocus();
            if (ImGui::Button("Cancel", ImVec2(ImGui::GetContentRegionAvail().x, 0))) { ImGui::CloseCurrentPopup(); }
            ImGui::EndPopup();
        }
    }
    ImGui::End();
}
