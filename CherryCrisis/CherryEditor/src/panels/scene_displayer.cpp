
#include "panels/scene_displayer.hpp"

#include "scene_manager.hpp"

#include <imgui.h>
#include <comdef.h>

#include "core/editor_manager.hpp"
#include "ImGuizmo.h"
#include "transform.hpp"

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
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(".cherry")) 
            {
                const char* c = (const char*)payload->Data;
                manager->m_selectedEntities.clear();
                SceneManager::GetInstance()->m_currentScene->Unserialize(c);
            }

            ImGui::EndDragDropTarget();
        }

        // matrices view and proj
        CCMaths::Matrix4 projection = Matrix4::Perspective(CCMaths::PI / 3.f, 4.f / 3.f, 0.01f, 200.f);
        // TODO: Replace with editor camera view matrix
        CCMaths::Matrix4 view = Matrix4::Scale({ 1.f,1.f,1.f }) * Matrix4::RotateZXY({ 0.f,0.f,0.f }) * Matrix4::Translate({ -1.f,1.f,-1.f });

        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();
        ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());
        if (manager->m_selectedEntities.size() > 0 && manager->m_selectedEntities[0]->GetBehaviour<Transform>() != nullptr) 
        {
            Transform* t = manager->m_selectedEntities[0]->GetBehaviour<Transform>();
            CCMaths::Matrix4 mat = t->GetWorldMatrix();
            float p[3], r[3], s[3]; // position, rotation and scale

            if (ImGuizmo::Manipulate(view.data, projection.data,
                ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::MODE::LOCAL, mat.data))
            {
                ImGuizmo::DecomposeMatrixToComponents(mat.data, p, r, s);

                t->SetPosition({p[0],p[1],p[2]}); 
                t->SetRotation({ r[0],r[1],r[2]}); 
                t->SetScale({s[0],s[1],s[2]});
            }            
        }
        ImGui::EndChild();
    }


    ImGui::End();
}