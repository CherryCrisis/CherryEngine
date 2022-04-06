#pragma once

#include "core/frame_displayer.hpp"
#include "camera.hpp"
#include "input_manager.hpp"

class EditorManager;
class SceneDisplayer : public FrameDisplayer
{
private:
	Camera m_camera = {};
	float m_cameraSpeed = 1.f;
	InputManager::InputContext* m_sceneContext = nullptr;

public:
	EditorManager* manager = nullptr;
	
	SceneDisplayer();
	
	void UpdateCamera();
	void Render() override;
};