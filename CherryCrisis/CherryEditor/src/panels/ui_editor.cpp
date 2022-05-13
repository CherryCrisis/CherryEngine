
#include "panels/ui_editor.hpp"

#include <imgui.h>

#include "camera_component.hpp"
#include <iostream>
#include "scene_manager.hpp"
#include "ui_context.hpp"
#include "core/imcherry.hpp"

#include "ui_item.hpp"

UIEditor::UIEditor() 
{
    m_items.push_back("Image");
    m_items.push_back("Horizontal Layout");
    m_items.push_back("Vertical Layout");
    m_items.push_back("Text");
    m_items.push_back("Progress Bar");
    m_items.push_back("Button");
    m_items.push_back("Toggle");
    m_items.push_back("Slider");
    m_items.push_back("Input Text");
}

void UIEditor::Render() 
{
	if (!m_isOpened)
		return;

	if (ImGui::Begin("UI Editor", &m_isOpened))
	{

        m_isActive = !ImGui::IsWindowCollapsed();

        // Add UI picking
        
        // Splits window into 3 panels : left is UI Items list, middle if main camera feedback with UI and right is item inspector
        uint64_t ViewTex = (uint64_t)m_framebuffer.colorTex.texID;

        UIContext& context = SceneManager::GetInstance()->m_currentScene->m_UIContext;

        if (ImGui::BeginTable("###EDITORUI", 3, ImGuiTableFlags_Resizable)) {
            std::vector<float> vals1{ 0.5, 1.5, 2.5 };
            std::vector<float> vals2{ 1.5, 2.5, 3.5 };

            ImGui::TableSetupColumn("UI Items");
            ImGui::TableSetupColumn("Scene");
            ImGui::TableSetupColumn("UI Inspector");
            ImGui::TableHeadersRow();

            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            // loop on every availables items
            ImGui::Text("Item Templates:");
            if (ImGui::BeginChild("###TemplatesUI", ImVec2(ImGui::GetContentRegionAvail().x, (ImGui::GetContentRegionAvail().y / 2.f) - 5.f), false))
            {
                for (int i = 0; i < m_items.size(); i++)
                {
                    ImGui::Selectable(m_items[i].c_str());
                    SetDraggedItem(m_items[i], (EItemUI)i);
                }
            }
            ImGui::EndChild();
            ImGui::Text("Hierarchy:");
            if (ImGui::BeginChild("###HierarchyUI", ImVec2(ImGui::GetContentRegionAvail().x, (ImGui::GetContentRegionAvail().y)), false))
            {
                for (UIItem* item : context.m_items)
                {
                    if (ImGui::Selectable(item->GetName().c_str(), m_selectedItem == item)) 
                    {
                        m_selectedItem = item;
                    }
                }
            }
            ImGui::EndChild();

            ImGui::TableSetColumnIndex(1);
            ImVec2 wsize = ImGui::GetContentRegionAvail();

            if (m_isActive && CameraComponent::GetMainCamera())
                if (Camera* cam = &CameraComponent::GetMainCamera()->m_camera)
                    UpdateFramebuffer(wsize.x, wsize.y, *cam);

            ImGui::Image((ImTextureID)ViewTex, wsize, ImVec2(0, 1), ImVec2(1, 0));
            //Add target source
            
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("UI_EDITOR_ITEM_TEMPLATE"))
                {
                    context.AddItemByType(*((EItemUI*)payload->Data));
                }

                ImGui::EndDragDropTarget();
            }

            ImGui::TableSetColumnIndex(2);
            // loop on every fields of the selected UI
            InspectSelectedItem();
            if (ImGui::Button("rm")) 
            {
                context.RemoveItem(m_selectedItem);
                m_selectedItem = nullptr;
            }

            ImGui::EndTable();
        }


        // Add UI gizmos
	}

	ImGui::End();
}

void UIEditor::SetDraggedItem(const std::string& item, EItemUI type) 
{
    if (ImGui::BeginDragDropSource())
    {
        ImGui::SetDragDropPayload("UI_EDITOR_ITEM_TEMPLATE", &type, sizeof(EItemUI), ImGuiCond_Once);
        ImGui::Text(item.c_str());
        ImGui::EndDragDropSource();
    }
}

void UIEditor::InspectSelectedItem()
{
    if (!m_selectedItem)
        return;
    ImVec4 color1 = { 100.f / 255.f, 10.f / 255.f, 10.f / 255.f, 1.f };
    ImVec4 color2 = { 18.f / 255.f, 120.f / 255.f, 4.f / 255.f, 1.f };
    ImVec4 color3 = { 12.f / 255.f, 50.f / 255.f, 170.f / 255.f, 1.f };

    // iterate on fields
    auto& fields = m_selectedItem->GetFields();
    for (auto& [fieldName, fieldRef] : fields)
    {
        const std::type_index type = fieldRef.m_value.type();

        if (type == typeid(CCMaths::Vector3))
        {
            CCMaths::Vector3 val = std::any_cast<CCMaths::Vector3>(fieldRef.m_value);
            ImCherry::ColoredDragFloat3(fieldRef.m_name.c_str(), val.data, { 1.f,1.f,1.f,1.f }, 0.5f);
            continue;
        }

        if (type == typeid(CCMaths::Vector3*))
        {
            CCMaths::Vector3* val = std::any_cast<CCMaths::Vector3*>(fieldRef.m_value);
            ImCherry::ColoredDragFloat3(fieldRef.m_name.c_str(), val->data, color1, color2, color3, 0.5f);
            continue;
        }

        if (type == typeid(CCMaths::Vector3**))
        {
            CCMaths::Vector3** val = std::any_cast<CCMaths::Vector3**>(fieldRef.m_value);
            CCMaths::Vector3* v = *val;
            ImCherry::ColoredDragFloat3(fieldRef.m_name.c_str(), v->data, color1, color2, color3, 0.5f);
            continue;
        }

        if (type == typeid(bool))
        {
            bool val = std::any_cast<bool>(fieldRef.m_value);
            ImGui::Checkbox(fieldRef.m_name.c_str(), &val);
            continue;
        }
        if (type == typeid(bool*))
        {
            bool* val = std::any_cast<bool*>(fieldRef.m_value);
            ImGui::Checkbox(fieldRef.m_name.c_str(), val);
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
            ImGui::DragFloat(fieldRef.m_name.c_str(), val, 0.5f, 0.0f, 0.0f, *val >= 1.0e+5 ? "%e" : "%.3f");
            continue;
        }
        if (type == typeid(float))
        {
            float val = std::any_cast<float>(fieldRef.m_value);
            ImGui::DragFloat(fieldRef.m_name.c_str(), &val, 0.5f, 0.0f, 0.0f, val >= 1.0e+5 ? "%e" : "%.3f");
            continue;
        }

        if (type == typeid(std::string*))
        {
            std::string val = *std::any_cast<std::string*>(fieldRef.m_value);
            ImGui::InputText(fieldRef.m_name.c_str(), &val[0], val.size() + 1);
            continue;
        }

        if (type == typeid(Object*))
        {
            Object* val = *std::any_cast<Object**>(fieldRef.m_value);

            ImGui::Text("%s", typeid(*val).name());

            continue;
        }
    }

    std::unordered_map <std::string, CCProperty::IClearProperty*>& properties = m_selectedItem->GetProperties();

    for (const auto& [propName, propRef] : properties)
    {
        auto& propType = propRef->GetGetType();

        if (propType == typeid(CCMaths::Vector3))
        {
            CCMaths::Vector3 val;
            propRef->Get(&val);
            if (ImCherry::ColoredDragFloat3(propName.c_str(), val.data, color1, color2, color3, 0.5f))
                propRef->Set(&val);

            continue;
        }

        if (propType == typeid(CCMaths::Vector2))
        {
            CCMaths::Vector2 val;
            propRef->Get(&val);
            if (ImGui::DragFloat2(propName.c_str(), val.data))
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

        if (propType == typeid(bool))
        {
            bool val;
            propRef->Get(&val);
            if (ImGui::Checkbox(propName.c_str(), &val))
                propRef->Set(&val);

            continue;
        }     

        if (propType == typeid(float))
        {
            float val;
            propRef->Get(&val);
            if (ImGui::DragFloat(propName.c_str(), &val, 0.5f, 0.0f, 0.0f, val >= 1.0e+5 ? "%e" : "%.3f"))
                propRef->Set(&val);


            continue;
        }

        if (propType == typeid(std::string))
        {
            std::string val;
            propRef->Get(&val);
            if (ImGui::InputText(propName.c_str(), &val[0], val.size() + 2))
                propRef->Set(&val);

            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("NODE"))
                {
                    const char* data = (const char*)payload->Data;
                    std::string strData = data;
                    propRef->Set(&strData);
                }
                ImGui::EndDragDropTarget();
            }

            continue;
        }

        if (propType == typeid(char*))
        {
            char* val;
            propRef->Get(&val);
            if (ImGui::InputText(propName.c_str(), val, strlen(val) + 2))
                propRef->Set(&val);

            continue;
        }

        if (propType == typeid(const char*))
        {
            const char* val = nullptr;
            propRef->Get(&val);

            ImGui::Text("%s %s", propName.c_str(), val ? val : "X");
            continue;
        }

        if (propType == typeid(Object*))
        {
            Object* val = nullptr;
            propRef->Get(&val);

            ImGui::Text("%s", propName.c_str());

            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_DROP"))
                {
                    Entity* m_draggedEntity = (Entity*)payload->Data;
                    propRef->Set(&m_draggedEntity);
                }

                ImGui::EndDragDropTarget();
            }

            continue;
        }
    }
}