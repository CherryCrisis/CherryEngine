#pragma once

#include "core/panel.hpp"

#include <unordered_map>

class Scene;
class EditorManager;
class Entity;
class CellSystemDisplayer;
class Transform;

class HierarchyDisplayer : public Panel
{
private:
	EditorManager* m_manager = nullptr;
	CellSystemDisplayer* m_cellSystemDisplayer = nullptr;

	Entity* m_draggedEntity = nullptr;
	bool	m_isEntityDragged = false;

	bool m_renaming = false;

	ImGuiIO* IO = nullptr;

public:
	void Render() override;
	void ContextCallback() override;

	bool RenderEntity(Entity* entity, Transform* transform = nullptr);

	HierarchyDisplayer(bool spawnOpened = false, EditorManager* manager = nullptr);
};