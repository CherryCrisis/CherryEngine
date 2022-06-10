#include "panels/scene_displayer.hpp"

#include <comdef.h>
#include <algorithm>

#include "resource_manager.hpp"
#include "scene_manager.hpp"
#include "time_manager.hpp"

#include "camera_component.hpp"
#include "core/editor_manager.hpp"
#include "maths.hpp"
#include "mixed_rendering_pipeline.hpp"
#include "model.hpp"
#include "pickinger.hpp"
#include "transform.hpp"
#include "utils.hpp"
#include "entity.hpp"
#include "portal_render_renderpass.hpp"

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
    CCMaths::Vector2 deltaMouse = InputManager::GetMouseDelta() * 0.005f;

    Vector3 rotation = m_camera.GetRotation();
    CCMaths::Matrix4 view = Matrix4::RotateYXZ(rotation);

    Vector3 up = Vector3::Up;
    Vector3 right = view.right;
    Vector3 forward = -view.back;

    float dt = TimeManager::GetDeltaTime();

    m_cameraSpeed += InputManager::GetMouseWheel().y * 0.5f;
    m_cameraSpeed = CCMaths::Clamp(m_cameraSpeed, 0.5f, 500.f);

    float speed = dt * m_cameraSpeed;

    // Compute each movement vector
    Vector3 forwardMove = forward * InputManager::GetAxis("FrontBack");
    Vector3 rightwardMove = right * InputManager::GetAxis("RightLeft");
    Vector3 upwardMove = up * InputManager::GetAxis("UpDown");

    m_camera.SetPitch(rotation.pitch + deltaMouse.y);
    m_camera.SetYaw(rotation.yaw + deltaMouse.x);
    
    m_camera.SetPosition(m_camera.GetPosition() + (forwardMove + rightwardMove + upwardMove) * speed);
}

void SceneDisplayer::Render() 
{
    if (!m_isOpened)
        return;

    if (!m_isFocused && InputManager::GetKeyDown("Save"))
        EditorNotifications::SceneSaving(SceneManager::SaveCurrentScene());

    m_isActive = true;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.f,0.f });
    if (ImGui::Begin("Scene", &m_isOpened, ImGuiWindowFlags_MenuBar))
    {
        RenderMenuBar();

        InputManager::PushContext("Editor Context");

        if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows)) 
        {
            if (InputManager::GetKeyDown(Keycode::RIGHT_CLICK)) { Focus(); }

            if (!m_manager->m_entitySelector.IsEmpty())
            {
                if (InputManager::GetKeyDown(Keycode::DEL))
                {
                    for (Entity* entity : m_manager->m_entitySelector.m_entities)
                        SceneManager::GetInstance()->m_currentScene->RemoveEntity(entity);

                    SceneManager::SetHierarchyDirty(true);

                    m_manager->m_entitySelector.Clear(false);
                }
            }
        }

        if (m_isFocused)
        {
            if (InputManager::GetKey(Keycode::RIGHT_CLICK)) { UpdateCamera(); }
            if (InputManager::GetKeyUp(Keycode::RIGHT_CLICK)) { Unfocus(); }
        }

        if (!InputManager::GetKey(Keycode::RIGHT_CLICK))
        {
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
            glBindFramebuffer(GL_FRAMEBUFFER, m_camera.m_framebuffer->FBO);
            Pickinger::SetBuffer(&m_camera);
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

        
        m_camera.SetSize({ wsize.x, wsize.y });
        m_camera.Draw(RECURSION_COUNT);

        uint64_t ViewTex = (uint64_t)m_camera.m_framebuffer->colorTex.texID;
        ImGui::Image((ImTextureID)ViewTex, wsize, ImVec2(0, 1), ImVec2(1, 0));

        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("NODE"))
            {
                const char* data = (const char*)payload->Data;
                std::string extension = String::ExtractLastValue(data, '.');

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
            
            Matrix4 deltaMatrix = Matrix4::Identity; // Not used
            Vector3 snapping = m_operation == ImGuizmo::ROTATE && m_isSnapping ? Vector3(m_rotSnap) : Vector3::Zero;
            if (ImGuizmo::Manipulate(view.data, projection.data, m_operation, m_mode, m_guizmoTransform.data, deltaMatrix.data, snapping.data))
            {
                Transform* parent = t->GetParent();
                CCMaths::Matrix4 mat = m_guizmoTransform;

                if (parent)
                    mat = CCMaths::Matrix4::Inverse(parent->GetWorldMatrix()) * mat;

                CCMaths::Vector3 p = mat.position;
                CCMaths::Vector3 s = Matrix4::GetScaleInMatrix(mat);
                CCMaths::Quaternion rot = Quaternion::FromMatrix(Matrix4::GetRotationMatrix(mat));
                
                if (m_isSnapping)
                {
                    Vector3 pos = t->GetPosition();

                    if (p.x != pos.x)
                        p.x = CCMaths::Round(p.x / m_posSnap) * m_posSnap;

                    if (p.y != pos.y)
                        p.y = CCMaths::Round(p.y / m_posSnap) * m_posSnap;

                    if (p.z != pos.z)
                        p.z = CCMaths::Round(p.z / m_posSnap) * m_posSnap;
                }

                t->SetPosition(p);
                t->SetRotation(rot);
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
    ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, { 0,0 });
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 8,8 });
    if (ImGui::BeginMenuBar())
    {
        bool m_isWorld = m_mode == ImGuizmo::MODE::WORLD;
        bool m_isLocal = m_mode == ImGuizmo::MODE::LOCAL;
        bool m_isTranslate = m_operation == ImGuizmo::TRANSLATE;
        bool m_isRotate    = m_operation == ImGuizmo::ROTATE;
        bool m_isScale     = m_operation == ImGuizmo::SCALE;

        float takenSpace = ImGui::CalcTextSize("Translate").x + ImGui::CalcTextSize("Rotate").x +
            ImGui::CalcTextSize("Scale").x + ImGui::CalcTextSize("Global").x + ImGui::CalcTextSize("Local").x +
            ImGui::CalcTextSize("Snap").x + 325.f;

        float emptySpace = ImGui::GetContentRegionAvail().x - takenSpace;

        if (ImGui::Selectable("Translate", &m_isTranslate, 0, ImGui::CalcTextSize("Translate")))
            m_operation = ImGuizmo::TRANSLATE;

        ImGui::Spacing();
        if (ImGui::Selectable("Rotate", &m_isRotate, 0, ImGui::CalcTextSize("Rotate")))
            m_operation = ImGuizmo::ROTATE;

        ImGui::Spacing();
        if (ImGui::Selectable("Scale", &m_isScale, 0, ImGui::CalcTextSize("Scale")))
            m_operation = ImGuizmo::SCALE;

        ImGui::Dummy({ emptySpace*(1.f / 10.f),0.f});
        if (ImGui::Selectable("Global", &m_isWorld, 0, ImGui::CalcTextSize("Global")))
            m_mode = ImGuizmo::MODE::WORLD;

        ImGui::Spacing();
        if (ImGui::Selectable("Local", &m_isLocal, 0, ImGui::CalcTextSize("Local")))
            m_mode = ImGuizmo::MODE::LOCAL;

        ImGui::Dummy({ emptySpace *(9.f/10.f),0.f });
        ImGui::Selectable("Snap", &m_isSnapping, 0, ImGui::CalcTextSize("Snap"));

        ImGui::Spacing();
        ImGui::SetNextItemWidth(50.f);
        ImGui::DragFloat("Position", &m_posSnap, 0.01f, 0.01f, 5.f, "%.2f");

        ImGui::Spacing();
        ImGui::SetNextItemWidth(50.f);
        ImGui::DragFloat("Rotation", &m_rotSnap, 0.1f, 0.1f, 90.f, "%.1f");
    }

    ImGui::EndMenuBar();

    ImGui::PopStyleVar(2);
}
