#pragma once

#include "core/frame_displayer.hpp"
#include "camera.hpp"
#include "cell_system.hpp"
#include "ImGuizmo.h"

class EditorManager;
class CellSystemDisplayer : public FrameDisplayer
{
private:
	Camera m_camera = {};
	float m_cameraSpeed = 1.f;

	CellSystem* m_cellSystem = nullptr;
	Cell* m_selectedCell = nullptr;
	Cell* m_rightClickedCell = nullptr;
	int m_selectedCategory = 0;

public:
	EditorManager* m_manager = nullptr;
	
	CellSystemDisplayer();
	
	void CreateCell();
	bool Context();

	void UpdateCamera();
	void Render() override;

	void Focus() override;
	void Unfocus() override;
};