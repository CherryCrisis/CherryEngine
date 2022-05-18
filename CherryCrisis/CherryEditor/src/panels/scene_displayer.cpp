
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

#include "mixed_rendering_pipeline.hpp"

#include "utils.hpp"
#include "camera_component.hpp"
#include "model.hpp"

#undef near
#undef far

SceneDisplayer::SceneDisplayer() 
{
    InputManager::PushContext("Editor Context");

    int i = 0;
    InputManager::AddActionAxes("UpDown", i);
    InputManager::AddAxisToAction("UpDown", { Keycode::SPACE, Keycode::LEFT_CONTROL });

    InputManager::AddActionAxes("FrontBack", i);
    InputManager::AddAxisToAction("FrontBack", { Keycode::W, Keycode::S });

    InputManager::AddActionAxes("RightLeft", i);
    InputManager::AddAxisToAction("RightLeft", { Keycode::D, Keycode::A });

    InputManager::AddActionSingle("Save", i);
    InputManager::AddInputToAction("Save", Keycode::S);
    InputManager::SetActionPriorKey("Save", EPriorKey::LEFT_CONTROL);

    InputManager::AddActionSingle("Translate", i);
    InputManager::AddActionSingle("Rotate", i);
    InputManager::AddActionSingle("Scale", i);

    InputManager::AddInputToAction("Translate", Keycode::W);
    InputManager::AddInputToAction("Rotate", Keycode::E);
    InputManager::AddInputToAction("Scale", Keycode::R);

    InputManager::AddActionSingle("World", i);
    InputManager::AddActionSingle("Local", i);
    InputManager::AddInputToAction("World", Keycode::T);
    InputManager::AddInputToAction("Local", Keycode::Y);

    InputManager::AddActionSingle("Pick", i);
    InputManager::AddInputToAction("Pick", Keycode::LEFT_CLICK);

    InputManager::PopContext();

    m_camera.m_pipeline = std::make_unique<MixedPipeline>();
    CameraComponent::m_editorCamera = &m_camera;
    if (Scene* scene = SceneManager::GetInstance()->m_currentScene.get())
    {
        scene->AddCell("Default")->AddViewer(&m_camera);
    }
}

SceneDisplayer::~SceneDisplayer()
{
    CameraComponent::m_editorCamera = nullptr;

    if (Scene* scene = SceneManager::GetInstance()->m_currentScene.get())
    {
        if (Cell* cell = m_manager->GetCellSystemDisplayer()->GetSelectedCell())
        {
            cell->RemoveViewer(&m_camera);
        }
    }
}

void SceneDisplayer::UpdateCamera()
{
    CCMaths::Vector2 deltaMouse = InputManager::GetMouseDelta();

    CCMaths::Matrix4 view = Matrix4::RotateYXZ(m_camera.rotation);

    Vector3 up = Vector3::Up;
    Vector3 right = view.right;
    Vector3 forward = -view.back;

    float dt = TimeManager::GetInstance()->GetDeltaTime();

    m_cameraSpeed += dt * InputManager::GetMouseWheel().y * 10.f;
    m_cameraSpeed = CCMaths::Clamp(m_cameraSpeed, 0.5f, 100.f);

    float speed = dt * m_cameraSpeed;

    // Compute each movement vector
    Vector3 forwardMove = forward * InputManager::GetAxis("FrontBack");
    Vector3 rightwardMove = right * InputManager::GetAxis("RightLeft");
    Vector3 upwardMove = up * InputManager::GetAxis("UpDown");

    m_camera.rotation.pitch += dt * deltaMouse.y;
    m_camera.rotation.yaw += dt * deltaMouse.x;
    
    m_camera.m_position += (forwardMove + rightwardMove + upwardMove) * speed;

    m_camera.m_viewMatrix = Matrix4::RotateXYZ(-m_camera.rotation) * Matrix4::Translate(-m_camera.m_position);
    m_camera.m_worldMatrix = Matrix4::Translate(m_camera.m_position) * Matrix4::RotateZYX(m_camera.rotation);
}

void SceneDisplayer::Render() 
{
    if (!m_isOpened)
        return;

    m_isActive = true;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.f,0.f });
    if (ImGui::Begin("Scene", &m_isOpened, ImGuiWindowFlags_MenuBar))
    {
        RenderMenuBar();

        InputManager::PushContext("Editor Context");

        if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows)) 
        {
            if (InputManager::GetKeyDown(Keycode::RIGHT_CLICK)) { Focus(); }
        }

        if (m_isFocused)
        {
            if (InputManager::GetKey(Keycode::RIGHT_CLICK)) { UpdateCamera(); }
            if (InputManager::GetKeyUp(Keycode::RIGHT_CLICK)) { Unfocus(); }
        }

        if (!InputManager::GetKey(Keycode::RIGHT_CLICK))
        {
            if (InputManager::GetKeyDown("Save"))
                EditorNotifications::SceneSaving(SceneManager::SaveCurrentScene());

            if (ImGui::IsWindowFocused(ImGuiHoveredFlags_ChildWindows))
            {
                if (InputManager::GetKeyDown("Translate"))   m_operation = ImGuizmo::OPERATION::TRANSLATE;
                if (InputManager::GetKeyDown("Rotate"))      m_operation = ImGuizmo::OPERATION::ROTATE;
                if (InputManager::GetKeyDown("Scale"))       m_operation = ImGuizmo::OPERATION::SCALE;

                if (InputManager::GetKeyDown("World"))       m_mode = ImGuizmo::MODE::WORLD;
                if (InputManager::GetKeyDown("Local"))       m_mode = ImGuizmo::MODE::LOCAL;
            }
        }

        EntitySelector& selector = m_manager->m_entitySelector;

        if (InputManager::GetKeyDown("Pick") && ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows)
            && !ImGuizmo::IsOver())
        {
            glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer.FBO);
            Pickinger::SetBuffer(&m_framebuffer, &m_camera);
            CCMaths::Vector2 mousePos = InputManager::GetMousePos();
            ImVec2 bufferPos = ImGui::GetWindowContentRegionMin();
            CCMaths::Vector2 mousebufferPos = { mousePos.x - (ImGui::GetWindowPos().x + bufferPos.x), mousePos.y - (ImGui::GetWindowPos().y + bufferPos.y) };
            Entity* pickedEntity = Pickinger::GetEntity(mousebufferPos);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            if (pickedEntity)
            {
                Entity* root = pickedEntity->GetBehaviour<Transform>()->GetRootParent()->GetHostPtr();
                if (m_manager->m_entitySelector.Contains(root))
                {
                    selector.Clear();
                    selector.Add(pickedEntity);
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
                const char* data = (const char*)payload->Data;
                std::string extension = String::ExtractValue(data, '.');

                if (sceneExtensions.compare('.' + extension) == 0)
                {
                    m_manager->m_entitySelector.Clear();
                    EditorNotifications::SceneLoading(SceneManager::LoadScene(data));
                }
                else if (modelExtensions.end() != modelExtensions.find("." + extension))
                {
                    std::shared_ptr<Model> model = ResourceManager::GetInstance()->GetResource<Model>(data);

                    if (Cell* cell = m_manager->GetCellSystemDisplayer()->GetSelectedCell())
                        SceneManager::GetInstance()->m_currentScene->GenerateEntitiesInCell(model, cell);

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

        if (!selector.IsEmpty() && selector.First()->GetBehaviour<Transform>())
        {
            Transform* t = selector.First()->GetBehaviour<Transform>();

            if (!m_isManipulated)
                m_guizmoTransform = t->GetWorldMatrix();

            if (ImGuizmo::Manipulate(view.data, projection.data, m_operation, m_mode, m_guizmoTransform.data))
            {
                Transform* parent = t->GetParent();
                CCMaths::Matrix4 mat = m_guizmoTransform;

                if (parent)
                    mat = CCMaths::Matrix4::Inverse(parent->GetWorldMatrix()) * mat;

                CCMaths::Vector3 p, r, s; // position, rotation and scale
                CCMaths::Matrix4::Decompose(mat, p, r, s);

                if (m_isSnapping)
                {
                    p.x = CCMaths::Round(p.x / m_posSnap) * m_posSnap;
                    p.y = CCMaths::Round(p.y / m_posSnap) * m_posSnap;
                    p.z = CCMaths::Round(p.z / m_posSnap) * m_posSnap;

                    r.x = CCMaths::Round(r.x / m_rotSnap) * m_rotSnap;
                    r.y = CCMaths::Round(r.y / m_rotSnap) * m_rotSnap;
                    r.z = CCMaths::Round(r.z / m_rotSnap) * m_rotSnap;
                }

                t->SetPosition(p);
                t->SetRotation(r);
                t->SetScale(s);
            }

            if (ImGuizmo::IsUsing() && InputManager::GetKey(Keycode::LEFT_CLICK))
                m_isManipulated = true;
            else if (m_isManipulated)
                m_isManipulated = false;
        }

        ImGui::EndChild();

        InputManager::PopContext();
    }

    ImGui::PopStyleVar(1);
    ImGui::End();
}

void SceneDisplayer::Focus()
{
    m_isFocused = true;
    InputManager::SetCursorHidden();
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
    ImGui::SetWindowFocus("Scene");
}

void SceneDisplayer::Unfocus()
{
    m_isFocused = false;
    InputManager::SetCursorDisplayed();
    ImGui::GetIO().ConfigFlags = ImGui::GetIO().ConfigFlags & ~ImGuiConfigFlags_NoMouse;
}

void SceneDisplayer::RenderMenuBar() 
{
    if (ImGui::BeginMenuBar())
    {
        bool m_isWorld = m_mode == ImGuizmo::MODE::WORLD;
        bool m_isLocal = m_mode == ImGuizmo::MODE::LOCAL;
        bool m_isTranslate = m_operation == ImGuizmo::TRANSLATE;
        bool m_isRotate    = m_operation == ImGuizmo::ROTATE;
        bool m_isScale     = m_operation == ImGuizmo::SCALE;

        if (ImGui::Selectable("Translate", &m_isTranslate, 0, ImGui::CalcTextSize("Clear on play")))
            m_operation = ImGuizmo::TRANSLATE;

        ImGui::Spacing();
        if (ImGui::Selectable("Rotate", &m_isRotate, 0, ImGui::CalcTextSize("AutoScroll")))
            m_operation = ImGuizmo::ROTATE;

        ImGui::Spacing();
        if (ImGui::Selectable("Scale", &m_isScale, 0, ImGui::CalcTextSize("Collapse")))
            m_operation = ImGuizmo::SCALE;

        ImGui::Spacing();
        if (ImGui::Selectable("Global", &m_isWorld, 0, ImGui::CalcTextSize("Collapse")))
            m_mode = ImGuizmo::MODE::WORLD;

        ImGui::Spacing();
        if (ImGui::Selectable("Local", &m_isLocal, 0, ImGui::CalcTextSize("Collapse")))
            m_mode = ImGuizmo::MODE::LOCAL;

        ImGui::Spacing();
        ImGui::Selectable("Snap", &m_isSnapping, 0, ImGui::CalcTextSize("Snap"));

        ImGui::Spacing();
        ImGui::SetNextItemWidth(50.f);
        ImGui::DragFloat("Position", &m_posSnap, 0.01f, 0.01f, 5.f, "%.2f");

        ImGui::Spacing();
        ImGui::SetNextItemWidth(50.f);
        float rot = m_rotSnap * CCMaths::RAD2DEG;
        if (ImGui::DragFloat("Rotation", &rot, 0.1f, 0.1f, 90.f, "%.1f"))
            m_rotSnap = rot * CCMaths::DEG2RAD;

    }

    ImGui::EndMenuBar();
}
