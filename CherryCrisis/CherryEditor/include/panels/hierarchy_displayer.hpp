#pragma once

#include <unordered_map>

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

		std::vector<HierarchyNode> m_childrens;
	};

	bool	m_isEntityDragged = false;
	bool	m_isRenaming = false;

	std::vector<HierarchyNode> m_nodes;

	CellSystemDisplayer*	m_cellSystemDisplayer = nullptr;
	Entity*					m_draggedEntity = nullptr;
	EditorManager*			m_manager = nullptr;

	ImGuiIO*	IO = nullptr;

public:
	HierarchyDisplayer(bool spawnOpened = false, EditorManager* manager = nullptr);

	void Render() override;
	void ContextCallback() override;

	void Refresh();

	////!!Deprecated!!////////////////////////////////
	bool RenderEntity(Entity* entity, Transform* transform = nullptr);
	/// //////////////////////////////////////////////
	bool RenderEntity(HierarchyNode node);
};