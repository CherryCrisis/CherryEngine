#include "panels/game_displayer.hpp"

#include <imgui.h>

#include "input_manager.hpp"
#include "scene_manager.hpp"

#include "camera_component.hpp"
#include "core/editor_manager.hpp"
#include "pickinger.hpp"
#include "portal_render_renderpass.hpp"

void GameDisplayer::Render() 
{
    if (!m_isOpened)
        return;

    m_isHovered = false;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.f,0.f });
    if (ImGui::Begin("Game", &m_isOpened))
    {
        if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows))
            m_isHovered = true;

        m_isActive = !ImGui::IsWindowCollapsed();

        ImGui::SetWindowSize({1920,1080});

        FrameDisplayer::Render();
        ImGui::BeginChild("GameFrameBuffer");
        ImVec2 wsize = ImGui::GetWindowSize();

        CameraComponent* camComp = CameraComponent::GetMainCamera();
        if (camComp)
        {
            Camera* mainCamera = &camComp->m_camera;

            if (m_manager->m_engine && Engine::isPlaying)
            {
                CCMaths::Vector2 mousePos = InputManager::GetInstance()->GetMousePos();

                ImVec2 bufferPos = ImGui::GetWindowContentRegionMin(); bufferPos.x;

                CCMaths::Vector2 mousebufferPos = { mousePos.x - (ImGui::GetWindowPos().x + bufferPos.x), mousePos.y - (ImGui::GetWindowPos().y + bufferPos.y) };
                CCMaths::Vector2 framebufferPos = { ImGui::GetWindowPos().x + bufferPos.x, ImGui::GetWindowPos().y + bufferPos.y };

                if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows)
                    && mousePos.x >= framebufferPos.x && mousePos.x <= framebufferPos.x + mainCamera->m_framebuffer->width
                    && mousePos.y >= framebufferPos.y && mousePos.y <= framebufferPos.y + mainCamera->m_framebuffer->height)
                {
                    glBindFramebuffer(GL_FRAMEBUFFER, mainCamera->m_framebuffer->FBO);
                    Pickinger::SetBuffer(mainCamera);
                    CCMaths::Vector2 mousePos = InputManager::GetMousePos();
                    ImVec2 bufferPos = ImGui::GetWindowContentRegionMin();
                    CCMaths::Vector2 mousebufferPos = { mousePos.x - (ImGui::GetWindowPos().x + bufferPos.x), mousePos.y - (ImGui::GetWindowPos().y + bufferPos.y) };

                    if (UIItem* item = Pickinger::GetUIItem(mousebufferPos))
                    {
                        item->SetHovered(true);

                        if (InputManager::GetInstance()->GetKeyDown(Keycode::LEFT_CLICK))
                            item->Interact();
                    }

                    glBindFramebuffer(GL_FRAMEBUFFER, 0);
                }
            }

            uint64_t viewTex;

            if (m_isActive && mainCamera)
            {
                mainCamera->SetSize({ wsize.x, wsize.y });
                mainCamera->Draw(RECURSION_COUNT);

                viewTex = (uint64_t)mainCamera->m_framebuffer->colorTex.texID;
            }

            ImGui::Image((ImTextureID)viewTex, wsize, ImVec2(0, 1), ImVec2(1, 0));
        }

        ImGui::EndChild();
    }
     
    ImGui::PopStyleVar(1);
    ImGui::End();
}

void GameDisplayer::Focus()
{
    InputManager::SetPollContext("User Context");
    if (!InputManager::IsUIFocused()) 
    {
        InputManager::SetCursorHidden();
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
    }

    m_isFocused = true;
    ImGui::SetWindowFocus("Game");
}

void GameDisplayer::Unfocus()
{
    InputManager::SetPollContext(nullptr);
    InputManager::SetCursorDisplayed();
    m_isFocused = false;
    ImGui::GetIO().ConfigFlags = ImGui::GetIO().ConfigFlags & ~ImGuiConfigFlags_NoMouse;
}
