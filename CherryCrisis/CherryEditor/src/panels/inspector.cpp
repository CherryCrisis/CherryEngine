
#include "panels/inspector.hpp"

#include <imgui.h>
#include <string>
#include <typeinfo>

#include "cherry_header.hpp"
#include "core/editor_manager.hpp"

#include "transform.hpp"
#include "light_component.hpp"
#include "camera_component.hpp"
#include "scripted_behaviour.hpp"
#include "csscripting_system.hpp"
#include "rigidbody.hpp"
#include "collider.hpp"

#define IMGUI_LEFT_LABEL(func, label, ...) (ImGui::TextUnformatted(label), ImGui::SameLine(), func("##" label, __VA_ARGS__))


Inspector::Inspector(bool spawnOpened, EditorManager* manager) : Panel(spawnOpened), m_manager(manager)
{
    
}

std::string ExtractValue(std::string str, const char key = ':')
{
    return str.substr(str.find_first_of(key) + 1);
}

void InspectComponents(Entity* entity, int id)
{
    std::vector<Behaviour*> behaviours = entity->GetAllBehaviours();

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

        bool opened = ImGui::TreeNode(bname.c_str());

        // check if right clicked
        if (InputManager::GetInstance()->GetKeyDown(Keycode::RIGHT_CLICK) && ImGui::IsItemHovered())
        {
            ImGui::OpenPopup("context");
        }

        if (opened)
        {
            auto& fields = behaviour->GetFields();
            for (auto& [fieldName, fieldRef] : fields)
            {
                const std::type_index type = fieldRef.m_value.type();

                if (type == typeid(CCMaths::Vector3))
                {
                    CCMaths::Vector3 val = std::any_cast<CCMaths::Vector3>(fieldRef.m_value);
                    ImGui::DragFloat3(fieldRef.m_name.c_str(), val.data, 0.5f);
                    continue;
                }

                if (type == typeid(CCMaths::Vector3*))
                {
                    CCMaths::Vector3* val = std::any_cast<CCMaths::Vector3*>(fieldRef.m_value);
                    ImGui::DragFloat3(fieldRef.m_name.c_str(), val->data, 0.5f);
                    continue;
                }

                if (type == typeid(CCMaths::Vector3**))
                {
                    CCMaths::Vector3** val = std::any_cast<CCMaths::Vector3**>(fieldRef.m_value);
                    CCMaths::Vector3* v = *val;
                    ImGui::DragFloat3(fieldRef.m_name.c_str(), v->data, 0.5f);
                    continue;
                }

                if (type == typeid(int*))
                {
                    int* val = std::any_cast<int*>(fieldRef.m_value);
                    ImGui::DragInt(fieldRef.m_name.c_str(), val, 0.5f);
                    continue;
                }
                if (type == typeid(float*))
                {
                    float* val = std::any_cast<float*>(fieldRef.m_value);
                    ImGui::DragFloat(fieldRef.m_name.c_str(), val, 0.5f);
                    continue;
                }
                if (type == typeid(float))
                {
                    float val = std::any_cast<float>(fieldRef.m_value);
                    ImGui::DragFloat(fieldRef.m_name.c_str(), &val, 0.5f);
                    continue;
                }

                if (type == typeid(std::string*))
                {
                    std::string val = *std::any_cast<std::string*>(fieldRef.m_value);
                    ImGui::InputText(fieldRef.m_name.c_str(), &val[0], val.size() + 1);
                    continue;
                }
            }

            std::unordered_map <std::string, CCProperty::IClearProperty*>& properties = behaviour->GetProperties();

            for (const auto& [propName, propRef] : properties)
            {
                auto& propType = propRef->GetGetType();

                if (propType == typeid(CCMaths::Vector3))
                {
                    CCMaths::Vector3 val;
                    propRef->Get(&val);
                    if (ImGui::DragFloat3(propName.c_str(), val.data, 0.5f))
                        propRef->Set(&val);

                    continue;
                }

                if (propType == typeid(int))
                {
                    int val;
                    propRef->Get(&val);
                    if (ImGui::DragInt(propName.c_str(), &val, 0.5f))
                        propRef->Set(&val);

                    continue;
                }

                if (propType == typeid(float))
                {
                    float val;
                    propRef->Get(&val);
                    if (ImGui::DragFloat(propName.c_str(), &val, 0.5f))
                        propRef->Set(&val);

                    continue;
                }

                if (propType == typeid(std::string))
                {
                    std::string val;
                    propRef->Get(&val);
                    if (ImGui::InputText(propName.c_str(), &val[0], val.size() + 2))
                        propRef->Set(&val);

                    continue;
                }
            }
            ImGui::TreePop();
        }
        ImGui::Separator();
        ImGui::PopID();
    }
}

void Inspector::Render() 
{
    if (!m_isOpened)
        return;

    if (ImGui::Begin("Inspector", &m_isOpened))
    {
        for (size_t i = 0; i < m_manager->m_selectedEntities.size(); i++)
        {
            InspectComponents(m_manager->m_selectedEntities[i], static_cast<int>(i));
        }
        if (m_manager->m_selectedEntities.size() > 0) 
        {
            if (ImGui::Button("Add Component", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
            {
                ImGui::OpenPopup("Add Component");
            }
        }


        ImVec2 center = { InputManager::GetInstance()->GetMousePos().x, InputManager::GetInstance()->GetMousePos().y };
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing);
        if (ImGui::BeginPopupModal("Add Component", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            static char search[32] = "";
            ImGui::InputText("##", search, IM_ARRAYSIZE(search));


            // TODO: Replace with list of available components
            if (ImGui::MenuItem("Transform"))      { m_manager->m_selectedEntities[0]->AddBehaviour<Transform>(); }
            if (ImGui::MenuItem("Camera"))     { }
            if (ImGui::MenuItem("Rigidbody")) { m_manager->m_selectedEntities[0]->AddBehaviour<Rigidbody>(); }
            if (ImGui::MenuItem("Collider"))  { m_manager->m_selectedEntities[0]->AddBehaviour<Collider>(); }
            for (const std::string& name : CsScriptingSystem::GetInstance()->classesName) 
            {
                if (ImGui::MenuItem(name.c_str()))
                {
                    ScriptedBehaviour* behaviour = m_manager->m_selectedEntities[0]->AddBehaviour<ScriptedBehaviour>();
                        behaviour->SetScriptClass(name);
                }
            }   

            //---------------------------------------------------

            ImGui::SetItemDefaultFocus();
            if (ImGui::Button("Cancel", ImVec2(ImGui::GetContentRegionAvail().x, 0))) { ImGui::CloseCurrentPopup(); }
            ImGui::EndPopup();
        }
    }
    ImGui::End();
}
