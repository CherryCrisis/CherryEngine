
#include "panels/scene_displayer.hpp"

#include "scene_manager.hpp"

#include <imgui.h>
#include <comdef.h>

#include "core/editor_manager.hpp"

void SceneDisplayer::Render() 
{
    if (!m_isOpened)
        return;

    m_isActive = true;

    if (ImGui::Begin("Scene", &m_isOpened))
    {
        m_isActive = !ImGui::IsWindowCollapsed();

        ImGui::BeginChild("SceneFrameBuffer");
        ImVec2 wsize = ImGui::GetWindowSize();
        
        if (m_isActive)
            UpdateFramebuffer(wsize.x, wsize.y);

        uint64_t ViewTex = (uint64_t)m_ViewTex;

        ImGui::Image((ImTextureID)ViewTex, wsize, ImVec2(0, 1), ImVec2(1, 0));

        if (ImGui::BeginDragDropTarget()) 
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_NODE")) 
            {
                //const wchar_t* path = (const wchar_t*)payload->Data;
                //_bstr_t b(path);
                const char* c = (const char*)payload->Data;
                manager->m_selectedEntities.clear();
                SceneManager::GetInstance()->m_currentScene->Unserialize(c);
            }

            ImGui::EndDragDropTarget();
        }
        ImGui::EndChild();
    }

    ImGui::End();
}