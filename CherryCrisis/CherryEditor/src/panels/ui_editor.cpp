#include "panels/ui_editor.hpp"

#include <iostream>

#include <imgui.h>

#include "camera_component.hpp"
#include "core/editor_manager.hpp"
#include "core/imcherry.hpp"
#include "pickinger.hpp"
#include "scene_manager.hpp"
#include "ui_context.hpp"
#include "ui_item.hpp"
#include "entity.hpp"

UIEditor::UIEditor() 
{
    m_items.push_back("Image");
    m_items.push_back("Text");
    m_items.push_back("Button");
    //m_items.push_back("Toggle");
    //m_items.push_back("Slider");
    //m_items.push_back("Input Text");
}

void UIEditor::Render() 
{
	if (!m_isOpened)
		return;

	if (ImGui::Begin("UI Editor", &m_isOpened))
	{
        m_isActive = !ImGui::IsWindowCollapsed();

        uint64_t viewTex = 0u;

        Camera* mainCamera = nullptr;
        if (CameraComponent* cameraComp = CameraComponent::GetMainCamera())
        {
            if (mainCamera = &cameraComp->m_camera)
            {
                // Splits window into 3 panels : left is UI Items list, middle if main camera feedback with UI and right is item inspector
                viewTex = (uint64_t)mainCamera->m_framebuffer->colorTex.texID;
            }
        }

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
                        m_selectedItem = item;
                }
            }
            ImGui::EndChild();
            ImVec2 tableOffset = ImGui::GetContentRegionMax();
            ImGui::TableSetColumnIndex(1);
            ImVec2 wsize = ImGui::GetContentRegionAvail();

            if (mainCamera)
            {
                CCMaths::Vector2 mousePos = InputManager::GetInstance()->GetMousePos();

                ImVec2 bufferPos = ImGui::GetWindowContentRegionMin(); bufferPos.x += tableOffset.x;
                float headerHeight = ImGui::TableGetHeaderRowHeight();

                CCMaths::Vector2 mousebufferPos = { mousePos.x - (ImGui::GetWindowPos().x + bufferPos.x), mousePos.y - (ImGui::GetWindowPos().y + bufferPos.y + headerHeight) };
                CCMaths::Vector2 framebufferPos = { ImGui::GetWindowPos().x + bufferPos.x, ImGui::GetWindowPos().y + bufferPos.y + headerHeight };

                if (InputManager::GetInstance()->GetKeyDown(Keycode::LEFT_CLICK)
                    && ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows)
                    && mousePos.x >= framebufferPos.x && mousePos.x <= framebufferPos.x + mainCamera->m_framebuffer->width
                    && mousePos.y >= framebufferPos.y && mousePos.y <= framebufferPos.y + mainCamera->m_framebuffer->height)
                {
                    glBindFramebuffer(GL_FRAMEBUFFER, mainCamera->m_framebuffer->FBO);
                    Pickinger::SetBuffer(mainCamera);
                    CCMaths::Vector2 mousebufferPos = { mousePos.x - framebufferPos.x, mousePos.y - framebufferPos.y };
                    UIItem* pickedUIItem = Pickinger::GetUIItem(mousebufferPos);
                    glBindFramebuffer(GL_FRAMEBUFFER, 0);
                    m_selectedItem = pickedUIItem;
                }
            }

            if (m_isActive && mainCamera)
            {
                mainCamera->SetSize({ wsize.x, wsize.y });
                mainCamera->Draw(1);

                ImGui::Image((ImTextureID)viewTex, wsize, ImVec2(0, 1), ImVec2(1, 0));
            }

            
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("UI_EDITOR_ITEM_TEMPLATE"))
                {
                    context.AddItemByType(*((EItemUI*)payload->Data));
                    ImGui::EndDragDropTarget();
                }
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
    auto& metapack = m_selectedItem->GetMetapack();
    for (auto& [metaname, metadata] : metapack)
    {
        const std::type_index& type = metadata->GetType();

        if (type == typeid(CCMaths::Vector3))
        {
            CCMaths::Vector3 defaultVal;
            CCMaths::Vector3* valPtr = &defaultVal;
            metadata->Get((void**)&valPtr);
            if (valPtr && ImCherry::ColoredDragFloat3(metaname, valPtr->data))
                metadata->Set(valPtr);
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
            if (valPtr && ImGui::InputText(metaname.c_str(), &(*valPtr)[0], valPtr->size() + 1))
                metadata->Set(valPtr);

            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("NODE"))
                {
                    const char* data = (const char*)payload->Data;
                    std::string strData = data;
                    metadata->Set(&strData);
                }

                ImGui::EndDragDropTarget();
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
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("NODE"))
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
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("NODE"))
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
}
