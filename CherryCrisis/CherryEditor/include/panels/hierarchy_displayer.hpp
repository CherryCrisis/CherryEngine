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
	class HierarchyNode 
	{
	public:
		Entity* m_entity = nullptr;
		Transform* m_entityTransform = nullptr;
		bool m_isRoot = false;
		std::string m_name = "";
		
		std::vector<HierarchyNode> m_childrens;
	};

	EditorManager* m_manager = nullptr;
	CellSystemDisplayer* m_cellSystemDisplayer = nullptr;

	Entity* m_draggedEntity = nullptr;
	bool	m_isDragging  = false;

	bool m_renaming = false;

	ImGuiIO* IO = nullptr;

	std::vector<HierarchyNode> m_nodes;

	void HandleShortcuts();
public:
	void Render() override;
	void ContextCallback() override;

	void Refresh();

	bool RenderEntity(const HierarchyNode& node);

	void EmptyDrop();
	HierarchyDisplayer(bool spawnOpened = false, EditorManager* manager = nullptr);
};