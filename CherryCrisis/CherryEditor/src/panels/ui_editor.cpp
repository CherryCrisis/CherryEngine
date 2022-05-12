
#include "panels/ui_editor.hpp"

#include <imgui.h>

#include "camera_component.hpp"

void UIEditor::Render() 
{
	if (!m_isOpened)
		return;

	if (ImGui::Begin("UI Editor", &m_isOpened))
	{

        m_isActive = !ImGui::IsWindowCollapsed();

        ImGui::BeginChild("SceneFrameBuffer");
        ImVec2 wsize = ImGui::GetWindowSize();

        if (m_isActive && CameraComponent::GetMainCamera())
            if (Camera* cam = &CameraComponent::GetMainCamera()->m_camera)
                UpdateFramebuffer(wsize.x, wsize.y, *cam);

        uint64_t ViewTex = (uint64_t)m_framebuffer.colorTex.texID;
        ImGui::Image((ImTextureID)ViewTex, wsize, ImVec2(0, 1), ImVec2(1, 0));
	}

	ImGui::End();
}