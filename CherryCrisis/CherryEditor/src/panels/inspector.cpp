
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
    std::vector<Behaviour*> behaviours = entity->GetBehaviours();

    for (Behaviour* behaviour : behaviours)
    {
        std::string bname = typeid(*behaviour).name();
        bname = ExtractValue(bname, ' ');

        std::unordered_map<std::string, Field>& fields = behaviour->GetFields();

        ImGui::PushID(behaviour->GetUUID());
        if (ImGui::TreeNode(bname.c_str()))
        {
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

                if (type == typeid(int))
                {
                    int val = std::any_cast<int>(fieldRef.m_value);
                    ImGui::DragInt(fieldRef.m_name.c_str(), &val, 0.5f);
                    continue;
                }
                if (type == typeid(float))
                {
                    float val = std::any_cast<float>(fieldRef.m_value);
                    ImGui::DragFloat(fieldRef.m_name.c_str(), &val, 0.5f);
                    continue;
                }

                if (type == typeid(std::string))
                {
                    std::string val = std::any_cast<std::string>(fieldRef.m_value);
                    ImGui::InputText(fieldRef.m_name.c_str(), &val[0], val.size() + 1);
                    continue;
                }

                if (type == typeid(std::string*))
                {
                    std::string val = *std::any_cast<std::string*>(fieldRef.m_value);
                    ImGui::InputText(fieldRef.m_name.c_str(), &val[0], val.size() + 1);
                    continue;
                }

            }
            ImGui::TreePop();
        }
        ImGui::PopID();
    }
}

void Inspector::Render() 
{
    if (!m_isOpened)
        return;

    if (ImGui::Begin("Inspector", &m_isOpened))
    {
        int i = 0;
        for (auto& entity : m_manager->m_selectedEntities)
        {
            InspectComponents(entity, i);
            i++;
        }
    }
    ImGui::End();
}
