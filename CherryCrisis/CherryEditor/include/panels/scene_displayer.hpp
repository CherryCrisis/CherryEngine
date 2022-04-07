#pragma once

#include "core/frame_displayer.hpp"
#include "camera.hpp"
#include "input_manager.hpp"
#include "ImGuizmo.h"

class EditorManager;
class SceneDisplayer : public FrameDisplayer
{
private:
	Camera m_camera = {};
	float m_cameraSpeed = 1.f;
	InputManager::InputContext* m_sceneContext = nullptr;

public:
	EditorManager* m_manager = nullptr;
	
	SceneDisplayer();
	
	void UpdateCamera();
	void Render() override;

	ImGuizmo::OPERATION m_operation = ImGuizmo::OPERATION::TRANSLATE;
};