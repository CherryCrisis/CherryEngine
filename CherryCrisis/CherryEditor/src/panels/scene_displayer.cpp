
#include "panels/scene_displayer.hpp"

#include "scene_manager.hpp"

#include <imgui.h>
#include <comdef.h>
#include <algorithm>

#include "time_manager.hpp"
#include "core/editor_manager.hpp"
#include "transform.hpp"
#include "maths.hpp"
#include "resource_manager.hpp"

#include "pickinger.hpp"
#include "cell_system.hpp"

#include "basic_rendering_pipeline.hpp"

#include "utils.hpp"

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

    IM->AddActionButtons("World", i);
    IM->AddActionButtons("Local", i);
    IM->AddInputToAction("World", Keycode::T);
    IM->AddInputToAction("Local", Keycode::Y);

    IM->AddActionButtons("Pick", i);
    IM->AddInputToAction("Pick", Keycode::LEFT_CLICK);

    IM->PopContext();

    m_camera.m_pipeline = std::make_unique<BasicRPipeline>();
    CellSystem::GetInstance()->AddOrGetCell("Default")->AddViewer(&m_camera);
}

SceneDisplayer::~SceneDisplayer()
{
    CellSystem::GetInstance()->AddOrGetCell("Default")->RemoveViewer(&m_camera);
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
    
    m_cameraSpeed += dt * IM->GetMouseWheel().y * 10.f;
    m_cameraSpeed = CCMaths::Clamp(m_cameraSpeed, 0.5f, 100.f);

    float speed = dt * m_cameraSpeed;

    // Compute each movement vector
    Vector3 forwardMove = forward * IM->GetAxis("FrontBack");
    Vector3 rightwardMove = right * IM->GetAxis("RightLeft");
    Vector3 upwardMove = up * IM->GetAxis("UpDown");

    m_camera.rotation.pitch += dt * deltaMouse.y;
    m_camera.rotation.yaw += dt * deltaMouse.x;
    
    m_camera.position += (forwardMove + rightwardMove + upwardMove) * speed;

    m_camera.m_viewMatrix = Matrix4::RotateZXY(-m_camera.rotation) * Matrix4::Translate(-m_camera.position);
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
        {
            if (IM->GetKeyDown(Keycode::RIGHT_CLICK)) { Focus(); }
        }

        if (m_isFocused)
        {
            if (IM->GetKey(Keycode::RIGHT_CLICK)) { UpdateCamera(); }
            if (IM->GetKeyUp(Keycode::RIGHT_CLICK)) { Unfocus(); }
        }

        if (!IM->GetKey(Keycode::RIGHT_CLICK))
        {
            if (IM->GetKeyDown("Save"))
                EditorNotifications::SceneSaving(SceneManager::SaveCurrentScene());

            if (ImGui::IsWindowFocused(ImGuiHoveredFlags_ChildWindows))
            {
                if (IM->GetKeyDown("Translate"))   m_operation = ImGuizmo::OPERATION::TRANSLATE;
                if (IM->GetKeyDown("Rotate"))      m_operation = ImGuizmo::OPERATION::ROTATE;
                if (IM->GetKeyDown("Scale"))       m_operation = ImGuizmo::OPERATION::SCALE;

                if (IM->GetKeyDown("World"))       m_mode = ImGuizmo::MODE::WORLD;
                if (IM->GetKeyDown("Local"))       m_mode = ImGuizmo::MODE::LOCAL;
            }
        }

        EntitySelector& selector = m_manager->m_entitySelector;

        if (InputManager::GetInstance()->GetKeyDown("Pick") && ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows)
            && !ImGuizmo::IsOver())
        {
            glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer.FBO);
            Pickinger::SetBuffer(&m_framebuffer, &m_camera);
            CCMaths::Vector2 mousePos = InputManager::GetInstance()->GetMousePos();
            Entity* e = Pickinger::GetEntity(mousePos.x - (ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMin().x), mousePos.y - (ImGui::GetWindowPos().y + ImGui::GetWindowContentRegionMin().y));
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            if (e) 
            {
                Entity* root = e->GetBehaviour<Transform>()->GetRootParent()->GetHostPtr();
                if (std::find(m_manager->m_selectedEntities.begin(), m_manager->m_selectedEntities.end(), root) != m_manager->m_selectedEntities.end() ||
                    (e->GetBehaviour<Transform>()->IsChildOf(root->GetBehaviour<Transform>()) && m_manager->m_selectedEntities.size() > 0 && m_manager->m_selectedEntities[0]->GetBehaviour<Transform>()->IsChildOf(root->GetBehaviour<Transform>())))
                {
                    selector.Clear();
                    selector.Add(e);
                }
                else 
                {
                    selector.Clear();
                    selector.Add(root);
                }
            }
            else 
                selector.Clear();
        }

        m_isActive = !ImGui::IsWindowCollapsed();

        ImGui::BeginChild("SceneFrameBuffer");
        ImVec2 wsize = ImGui::GetWindowSize();
        
        if (m_isActive)
            UpdateFramebuffer(wsize.x, wsize.y, m_camera);

        uint64_t ViewTex = (uint64_t)m_framebuffer.colorTex.texID;
        ImGui::Image((ImTextureID)ViewTex, wsize, ImVec2(0, 1), ImVec2(1, 0));
 
        if (ImGui::BeginDragDropTarget()) 
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("NODE")) 
            {
                const char* c = (const char*)payload->Data;
                std::string extension = String::ExtractValue(c, '.');

                if (extension == "cherry") 
                {
                    m_manager->m_selectedEntities.clear();
                    EditorNotifications::SceneLoading(SceneManager::LoadScene(c));
                }
                else if (extension == "obj" || extension == "fbx" || extension == "gltf")
                {
                    auto cb = CCCallback::BindCallback(&Scene::GenerateEntities, SceneManager::GetInstance()->m_currentScene.get());
                    ResourceManager::GetInstance()->AddResourceMultiThreads<ModelBase>(c, true, cb);
                    EditorManager::SendNotification("Adding object ...", ENotifType::Info);
                }
            }
            ImGui::EndDragDropTarget();
        }

        CCMaths::Matrix4 projection = m_camera.m_projectionMatrix;
        CCMaths::Matrix4 view = m_camera.m_viewMatrix;

        {
            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist();
            ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());
        }
        if (!selector.IsEmpty() && selector.m_entities[0]->GetBehaviour<Transform>())
        {
            Transform* t = selector.m_entities[0]->GetBehaviour<Transform>();
            CCMaths::Matrix4 mat = t->GetWorldMatrix();

            if (ImGuizmo::Manipulate(view.data, projection.data, m_operation, m_mode, mat.data))
            {
                Transform* parent = t->GetParent();

                if (parent)
                    mat = CCMaths::Matrix4::Inverse(parent->GetWorldMatrix()) * mat;

                CCMaths::Vector3 p, r, s; // position, rotation and scale
                CCMaths::Matrix4::Decompose(mat, p, r, s);

                t->SetPosition(p); 
                t->SetRotation(r);
                t->SetScale(s);
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
