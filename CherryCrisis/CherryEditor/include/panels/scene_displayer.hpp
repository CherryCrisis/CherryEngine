#pragma once

#include <imgui.h>
#include <ImGuizmo.h>

#include "camera.hpp"
#include "core/frame_displayer.hpp"
#include "input_manager.hpp"
#include "maths.hpp"

class EditorManager;


class SceneDisplayer : public FrameDisplayer
{
private:
	bool	m_isManipulated = false;
	bool	m_isSnapping = true;
	float	m_cameraSpeed = 1.f;
	float	m_posSnap = 1.0f;
	float	m_rotSnap = 5.0f;
	
	CCMaths::Matrix4 m_guizmoTransform = CCMaths::Matrix4::Identity;

public:
	Camera m_camera = {};

	ImGuizmo::OPERATION m_operation = ImGuizmo::OPERATION::TRANSLATE;
	ImGuizmo::MODE		m_mode		= ImGuizmo::MODE::WORLD;

	EditorManager* m_manager = nullptr;
	
	SceneDisplayer();
	~SceneDisplayer();
	
	void UpdateCamera();
	void Render() override;
	void RenderMenuBar();

	void Focus() override;
	void Unfocus() override;
};