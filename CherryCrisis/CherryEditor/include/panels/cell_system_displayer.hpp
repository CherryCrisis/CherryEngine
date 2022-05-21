#pragma once

#include "core/panel.hpp"
#include "ImGuizmo.h"
#include "scene.hpp"

class EditorManager;
class Camera;


class CellSystemDisplayer : public Panel
{
private:
	bool	m_renameCell = false;
			
	Scene*	m_scene = nullptr;
	Cell*	m_displayedCell = nullptr;
	Cell*	m_selectedCell = nullptr;
	Cell*	m_rightClickedCell = nullptr;

	void SelectCell(Cell* cell);

public:
	EditorManager*	m_manager = nullptr;
	Camera*			m_camera = nullptr;
	
	CellSystemDisplayer(EditorManager* manager);

	Cell* GetSelectedCell() { return m_selectedCell; }

	void CellSettings();
	void CreateCell();
	void RenameCell();
	bool Context();

	void UpdateCamera();
	void Render() override;
	void RenderCells();

	void InvalidatePointers();
};