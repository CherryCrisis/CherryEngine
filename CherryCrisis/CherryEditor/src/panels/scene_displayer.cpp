
#include "panels/scene_displayer.hpp"

#include "scene_manager.hpp"

#include <imgui.h>
#include <comdef.h>

#include "time_manager.hpp"
#include "core/editor_manager.hpp"
#include "transform.hpp"
#include "maths.hpp"
#include "resource_manager.hpp"

#include "pickinger.hpp"

#undef near
#undef far

SceneDisplayer::SceneDisplayer() 
{
    InputManager* IM = InputManager::GetInstance();
    IM->PushContext("Editor Context");

    int i = 0;
    IM->AddActionAxes("UpDown", i);
    IM->AddAxisToAction("UpDown", { Keycode::SPACE, Keycode::LEFT_CONTROL });

    IM->AddActionAxes("FrontBack", i);
    IM->AddAxisToAction("FrontBack", { Keycode::W, Keycode::S });

    IM->AddActionAxes("RightLeft", i);
    IM->AddAxisToAction("RightLeft", { Keycode::D, Keycode::A });

    IM->AddActionButtons("Save", i);
    IM->AddInputToAction("Save", Keycode::S);
    IM->SetActionPriorKey("Save", EPriorKey::LEFT_CONTROL);

    IM->AddActionButtons("Translate", i);
    IM->AddActionButtons("Rotate", i);
    IM->AddActionButtons("Scale", i);

    IM->AddInputToAction("Translate", Keycode::W);
    IM->AddInputToAction("Rotate", Keycode::E);
    IM->AddInputToAction("Scale", Keycode::R);

    IM->AddActionButtons("Pick", i);
    IM->AddInputToAction("Pick", Keycode::LEFT_CLICK);

    IM->PopContext();
}

void SceneDisplayer::UpdateCamera()
{
    InputManager* IM = InputManager::GetInstance();
    CCMaths::Vector2 deltaMouse = IM->GetMouseDelta();

    CCMaths::Matrix4 view = Matrix4::RotateYXZ(m_camera.rotation);

    Vector3 up = Vector3::Up;
    Vector3 right = view.right;
    Vector3 forward = -view.back;

    float dt = TimeManager::GetInstance()->GetDeltaTime();
    float speed = dt * m_cameraSpeed;

    // Compute each movement vector
    Vector3 forwardMove = forward * IM->GetAxis("FrontBack");
    Vector3 rightwardMove = right * IM->GetAxis("RightLeft");
    Vector3 upwardMove = up * IM->GetAxis("UpDown");

    m_camera.rotation.pitch += dt * deltaMouse.y;
    m_camera.rotation.yaw += dt * deltaMouse.x;
    
    m_camera.position += (forwardMove + rightwardMove + upwardMove) * speed;
}

void SceneDisplayer::Render() 
{
    if (!m_isOpened)
        return;

    m_isActive = true;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.f,0.f });
    if (ImGui::Begin("Scene", &m_isOpened))
    {
        InputManager* IM = InputManager::GetInstance();

        IM->PushContext("Editor Context");

        if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows))
            if (IM->GetKeyDown(Keycode::RIGHT_CLICK)) { Focus(); }
        
        if (m_isFocused) 
        {
            if (IM->GetKey(Keycode::RIGHT_CLICK)) { UpdateCamera(); }
            if (IM->GetKeyUp(Keycode::RIGHT_CLICK)) { Unfocus(); }
        }

        if (ImGui::IsWindowFocused(ImGuiHoveredFlags_ChildWindows) && !IM->GetKey(Keycode::RIGHT_CLICK))
        {
            if (IM->GetKeyDown("Translate"))   m_operation = ImGuizmo::OPERATION::TRANSLATE;
            if (IM->GetKeyDown("Rotate"))      m_operation = ImGuizmo::OPERATION::ROTATE;
            if (IM->GetKeyDown("Scale"))       m_operation = ImGuizmo::OPERATION::SCALE;
        }

        if (IM->GetKeyDown("Save"))      EditorNotifications::SceneSaving(SceneManager::SaveCurrentScene());

        if (InputManager::GetInstance()->GetKeyDown("Pick") && ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows)
            && !ImGuizmo::IsOver())
        {
            glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer.FBO);
            Pickinger::SetBuffer(&m_framebuffer, &m_camera);
            CCMaths::Vector2 mousePos = InputManager::GetInstance()->GetMousePos();
            Entity* e = Pickinger::GetEntity(mousePos.x - (ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMin().x), mousePos.y - (ImGui::GetWindowPos().y + ImGui::GetWindowContentRegionMin().y));
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            //TODO: Add multi select CTRL 

            m_manager->m_selectedEntities.clear();
            if (e)
                m_manager->m_selectedEntities.push_back(e);

        }


        m_isActive = !ImGui::IsWindowCollapsed();

        ImGui::BeginChild("SceneFrameBuffer");
        ImVec2 wsize = ImGui::GetWindowSize();
        
        if (m_isActive)
            UpdateFramebuffer(wsize.x, wsize.y, m_camera);

        uint64_t ViewTex = (uint64_t)m_framebuffer.TexID;

        ImGui::Image((ImTextureID)ViewTex, wsize, ImVec2(0, 1), ImVec2(1, 0));
 
        if (ImGui::BeginDragDropTarget()) 
        {
       
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(".cherry")) 
            {
                const char* c = (const char*)payload->Data;
                m_manager->m_selectedEntities.clear();

                SceneManager::LoadScene(c);
            }
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(".obj") )
            {
                const char* c = (const char*)payload->Data;
                std::string str = "Assets/" + std::string(c);
                const char* string = str.c_str();
                auto cb = CCCallback::BindCallback(&Scene::GenerateEntities, SceneManager::GetInstance()->m_currentScene.get());
                ResourceManager::GetInstance()->AddResourceMultiThreads<ModelBase>(string, true, cb);
                EditorManager::SendNotification("Adding object ...", ENotifType::Info);
            }
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(".fbx"))
            {
                const char* c = (const char*)payload->Data;
                std::string str = "Assets/" + std::string(c);
                const char* string = str.c_str();
                auto cb = CCCallback::BindCallback(&Scene::GenerateEntities, SceneManager::GetInstance()->m_currentScene.get());
                ResourceManager::GetInstance()->AddResourceMultiThreads<ModelBase>(string, true, cb);
                EditorManager::SendNotification("Adding object ...", ENotifType::Info);
            }
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(".gltf"))
            {
                const char* c = (const char*)payload->Data;
                std::string str = "Assets/" + std::string(c);
                const char* string = str.c_str();
                auto cb = CCCallback::BindCallback(&Scene::GenerateEntities, SceneManager::GetInstance()->m_currentScene.get());
                ResourceManager::GetInstance()->AddResourceMultiThreads<ModelBase>(string, true, cb);
                EditorManager::SendNotification("Adding object ...", ENotifType::Info);
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
                CCMaths::Vector3 rot = { r[0]* CCMaths::DEG2RAD, r[1]* CCMaths::DEG2RAD,r[2]* CCMaths::DEG2RAD };
                
                t->SetPosition({p[0],p[1],p[2]}); 
                t->SetRotation(rot); 
                t->SetScale({s[0],s[1],s[2]});
            }            
        }
        ImGui::EndChild();

        IM->PopContext();
    }

    ImGui::PopStyleVar(1);
    ImGui::End();
}

void SceneDisplayer::Focus()
{
    m_isFocused = true;
    m_inputs->SetCursorHidden();
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
    ImGui::SetWindowFocus("Scene");
}

void SceneDisplayer::Unfocus()
{
    m_isFocused = false;
    m_inputs->SetCursorDisplayed();
    ImGui::GetIO().ConfigFlags = ImGui::GetIO().ConfigFlags & ~ImGuiConfigFlags_NoMouse;
}
