#pragma once

#include "core/frame_displayer.hpp"
#include "camera.hpp"
#include "input_manager.hpp"
#include "ImGuizmo.h"

class EditorManager;

class SceneDisplayer : public FrameDisplayer
{
private:
	float m_cameraSpeed = 1.f;

public:
	Camera m_camera = {};

	EditorManager* m_manager = nullptr;
	
	SceneDisplayer();
	~SceneDisplayer();
	
	void UpdateCamera();
	void Render() override;

	void Focus() override;
	void Unfocus() override;

	ImGuizmo::OPERATION m_operation = ImGuizmo::OPERATION::TRANSLATE;
	ImGuizmo::MODE		m_mode		= ImGuizmo::MODE::WORLD;
};