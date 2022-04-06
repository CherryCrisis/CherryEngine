
#include "panels/scene_displayer.hpp"

#include "scene_manager.hpp"

#include <imgui.h>
#include <comdef.h>

#include "time_manager.hpp"
#include "core/editor_manager.hpp"
#include "ImGuizmo.h"
#include "transform.hpp"

#undef near
#undef far

SceneDisplayer::SceneDisplayer() 
{
    InputManager* IM = InputManager::GetInstance();
    m_sceneContext = IM->AddContext("Scene Context");
    IM->SetUpdatedContext(m_sceneContext);

    int i = 0;
    IM->AddActionAxes("UpDown", i);
    IM->AddAxisToAction("UpDown", { Keycode::SPACE, Keycode::LEFT_CONTROL });

    IM->AddActionAxes("FrontBack", i);
    IM->AddAxisToAction("FrontBack", { Keycode::W, Keycode::S });

    IM->AddActionAxes("RightLeft", i);
    IM->AddAxisToAction("RightLeft", { Keycode::D, Keycode::A });
    
    IM->SetUpdatedContext(nullptr);
}

void SceneDisplayer::UpdateCamera()
{
    InputManager* IM = InputManager::GetInstance();
    float dt = TimeManager::GetInstance()->GetDeltaTime();
    float speed = dt * m_cameraSpeed;
    m_camera.position.x += IM->GetAxis("RightLeft") * speed;
    m_camera.position.y += IM->GetAxis("UpDown") * speed;
    m_camera.position.z += IM->GetAxis("FrontBack") * speed;

    CCMaths::Vector2 deltaMouse = IM->GetMouseDelta();

    m_camera.rotation.x += dt * deltaMouse.y;
    m_camera.rotation.y += dt * deltaMouse.x;
}

void SceneDisplayer::Render() 
{
    if (!m_isOpened)
        return;

    m_isActive = true;

    if (ImGui::Begin("Scene", &m_isOpened))
    {
        InputManager* IM = InputManager::GetInstance();
        IM->SetGetContext(m_sceneContext);
        
        if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows) && IM->GetKey(Keycode::RIGHT_CLICK))
            UpdateCamera();

        IM->SetGetContext(nullptr);

        m_isActive = !ImGui::IsWindowCollapsed();

        ImGui::BeginChild("SceneFrameBuffer");
        ImVec2 wsize = ImGui::GetWindowSize();
        
        if (m_isActive)
            UpdateFramebuffer(wsize.x, wsize.y, m_camera);

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

        CCMaths::Matrix4 projection = Matrix4::Perspective(m_camera.fovY, m_camera.aspect, m_camera.near, m_camera.far);
        CCMaths::Matrix4 view = Matrix4::RotateZXY(-m_camera.rotation) * Matrix4::Translate(-m_camera.position);

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