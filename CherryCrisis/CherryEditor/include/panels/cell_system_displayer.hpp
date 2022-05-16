#pragma once

#include "core/panel.hpp"

#include "scene.hpp"
#include "ImGuizmo.h"

class EditorManager;
struct Camera;

class CellSystemDisplayer : public Panel
{
private:
	Scene*		m_scene = nullptr;
	Cell*		m_displayedCell = nullptr;
	Cell*		m_selectedCell = nullptr;
	Cell*		m_rightClickedCell = nullptr;
	Entity*		m_selectedEntity = nullptr;
	bool		m_renameCell = false;

	void SelectCell(Cell* cell);

public:
	EditorManager* m_manager = nullptr;
	Camera* m_camera = nullptr;
	
	CellSystemDisplayer();

	Cell* GetSelectedCell() { return m_selectedCell; }
	

	void CellSettings();
	void CreateCell();
	void RenameCell();
	bool Context();

	void UpdateCamera();
	void Render() override;
	void RenderCells();
	void RenderEntities();

};