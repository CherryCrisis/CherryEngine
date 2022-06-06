#pragma once

#include <unordered_map>
#include <memory>

#include "core/panel.hpp"

class CellSystemDisplayer;
class EditorManager;
class Entity;
class Scene;
class Transform;


class HierarchyDisplayer : public Panel
{
private:
	class HierarchyNode 
	{
	public:
		Entity*		m_entity = nullptr;
		Transform*	m_entityTransform = nullptr;
		
		bool		m_isRoot = false;
		std::string m_name = "";

		std::vector<std::shared_ptr<HierarchyNode>> m_childrens;
	};

	bool	m_isDragging = false;
	bool	m_isRenaming = false;

	std::vector<std::shared_ptr<HierarchyNode>> m_nodes;

	CellSystemDisplayer*	m_cellSystemDisplayer = nullptr;
	Entity*					m_draggedEntity = nullptr;
	EditorManager*			m_manager = nullptr;

	ImGuiIO*	IO = nullptr;

	void HandleShortcuts();
public:
	HierarchyDisplayer(bool spawnOpened = false, EditorManager* manager = nullptr);

	void Render() override;
	void ContextCallback() override;

	void Refresh();

	bool RenderEntity(HierarchyNode* node);

	void EmptyDrop();
};
