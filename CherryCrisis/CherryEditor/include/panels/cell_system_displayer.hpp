#pragma once

#include "core/panel.hpp"

#include "cell_system.hpp"
#include "ImGuizmo.h"

class EditorManager;
class Camera;
class CellSystemDisplayer : public Panel
{
private:

	CellSystem* m_cellSystem = nullptr;
	Cell*		m_selectedCell = nullptr;
	Cell*		m_rightClickedCell = nullptr;
	Entity*		m_selectedEntity = nullptr;
	bool m_renameCell = false;

public:
	EditorManager* m_manager = nullptr;
	Camera* m_camera = nullptr;
	
	CellSystemDisplayer();
	
	void CreateCell();
	void RenameCell();
	bool Context();

	void UpdateCamera();
	void Render() override;
	void RenderCells();
	void RenderEntities();
};