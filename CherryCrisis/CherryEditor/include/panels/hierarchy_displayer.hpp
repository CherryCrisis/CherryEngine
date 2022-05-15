#pragma once

#include "core/panel.hpp"

#include <unordered_map>

class Scene;
class EditorManager;
class Entity;
class CellSystemDisplayer;

class HierarchyDisplayer : public Panel
{
private:
	EditorManager* m_manager = nullptr;
	CellSystemDisplayer* m_cellSystemDisplayer = nullptr;

	Entity* m_draggedEntity = nullptr;
	bool	m_isEntityDragged = false;

	bool m_renaming = false;

public:
	void Render() override;
	void ContextCallback() override;

	bool RenderEntity(Entity* entity);

	HierarchyDisplayer(bool spawnOpened = false, EditorManager* manager = nullptr);
};