#pragma once

#include "core/frame_displayer.hpp"
#include "camera.hpp"
#include "input_manager.hpp"
#include "ImGuizmo.h"

#include "maths.hpp"

class EditorManager;


class SceneDisplayer : public FrameDisplayer
{
private:
	float m_cameraSpeed = 1.f;

	float	m_posSnap = 1.0f;
	float	m_rotSnap = 5.0f * CCMaths::DEG2RAD;
	bool	m_isManipulated = false;
	bool	m_isSnapping = true;
	
	CCMaths::Matrix4 m_guizmoTransform = CCMaths::Matrix4::Identity;

public:
	Camera m_camera = {};

	EditorManager* m_manager = nullptr;
	
	SceneDisplayer();
	~SceneDisplayer();
	
	void UpdateCamera();
	void Render() override;
	void RenderMenuBar();

	void Focus() override;
	void Unfocus() override;

	ImGuizmo::OPERATION m_operation = ImGuizmo::OPERATION::TRANSLATE;
	ImGuizmo::MODE		m_mode		= ImGuizmo::MODE::WORLD;
};