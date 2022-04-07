
#include "panels/scene_displayer.hpp"

#include "scene_manager.hpp"

#include <imgui.h>
#include <comdef.h>

#include "time_manager.hpp"
#include "core/editor_manager.hpp"
#include "transform.hpp"
#include "callback.hpp"
#include "maths.hpp"

#undef near
#undef far

void Serialize() 
{
    if (SceneManager::GetInstance()->m_currentScene->Serialize("scn")) 
    {
        EditorManager::SendNotification("Scene Saved!", ImGuiToastType::Info);
    }
}

SceneDisplayer::SceneDisplayer() 
{
    InputManager* IM = InputManager::GetInstance();

    int i = 0;
    IM->AddActionAxes("UpDown", i);
    IM->AddAxisToAction("UpDown", { Keycode::SPACE, Keycode::LEFT_CONTROL });

    IM->AddActionAxes("BackFront", i);
    IM->AddAxisToAction("BackFront", { Keycode::S, Keycode::W });

    IM->AddActionAxes("RightLeft", i);
    IM->AddAxisToAction("RightLeft", { Keycode::D, Keycode::A });

    auto callback = CCCallback::BindCallback(Serialize);
    IM->AddActionButtons("Save",i)->m_pressed.Bind(callback);
    IM->AddInputToAction("Save", Keycode::S);
    IM->SetActionPriorKey("Save", EPriorKey::LEFT_CONTROL);

    IM->AddActionButtons("Translate", i);
    IM->AddActionButtons("Rotate", i);
    IM->AddActionButtons("Scale", i);

    IM->AddInputToAction("Translate", Keycode::W);
    IM->AddInputToAction("Rotate", Keycode::E);
    IM->AddInputToAction("Scale", Keycode::R);
}

void SceneDisplayer::UpdateCamera()
{
    InputManager* IM = InputManager::GetInstance();
    CCMaths::Vector2 deltaMouse = IM->GetMouseDelta();

    float dt = TimeManager::GetInstance()->GetDeltaTime();
    float speed = dt * m_cameraSpeed;

    m_camera.rotation.x += dt * deltaMouse.y;
    m_camera.rotation.y += dt * deltaMouse.x;
    
    m_camera.position.x += IM->GetAxis("RightLeft") * speed;// * transform.right ;
    m_camera.position.y += IM->GetAxis("UpDown") * speed;
    m_camera.position.z += IM->GetAxis("BackFront") * speed; // * transform.forward ;

}

void SceneDisplayer::Render() 
{
    if (!m_isOpened)
        return;

    m_isActive = true;

    if (ImGui::Begin("Scene", &m_isOpened))
    {
        InputManager* IM = InputManager::GetInstance();

        if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows))
        {
            if (IM->GetKeyDown(Keycode::RIGHT_CLICK)) { IM->SetCursorHidden(); }
            if (IM->GetKey(Keycode::RIGHT_CLICK))     { UpdateCamera(); }
            if (IM->GetKeyUp(Keycode::RIGHT_CLICK))   { IM->SetCursorDisplayed(); }
        }

        if (IM->GetKeyDown(Keycode::W))  m_operation = ImGuizmo::OPERATION::TRANSLATE; 
        if (IM->GetKey(Keycode::E))      m_operation = ImGuizmo::OPERATION::ROTATE; 
        if (IM->GetKeyUp(Keycode::R))    m_operation = ImGuizmo::OPERATION::SCALE; 

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
                m_manager->m_selectedEntities.clear();
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
        if (m_manager->m_selectedEntities.size() > 0 && m_manager->m_selectedEntities[0]->GetBehaviour<Transform>() != nullptr) 
        {
            Transform* t = m_manager->m_selectedEntities[0]->GetBehaviour<Transform>();
            CCMaths::Matrix4 mat = t->GetWorldMatrix();
            float p[3], r[3], s[3]; // position, rotation and scale

            if (ImGuizmo::Manipulate(view.data, projection.data, m_operation, ImGuizmo::MODE::LOCAL, mat.data))
            {
                ImGuizmo::DecomposeMatrixToComponents(mat.data, p, r, s);
                
                //TODO: Fix operator*(const float) not working
                CCMaths::Vector3 rot = { r[0]* CCMaths::DEG2RAD ,r[1]* CCMaths::DEG2RAD,r[2]* CCMaths::DEG2RAD };
                
                t->SetPosition({p[0],p[1],p[2]}); 
                t->SetRotation(rot); 
                t->SetScale({s[0],s[1],s[2]});
            }            
        }
        ImGui::EndChild();
    }


    ImGui::End();
}