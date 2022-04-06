#pragma once

#include "core/panel.hpp"

class Scene;
class EditorManager;
class Entity;

class HierarchyDisplayer : public Panel 
{
private:
	EditorManager* m_manager = nullptr;

	bool m_renaming = false;

public:
	void Render() override;
	void ContextCallback() override;

	bool RenderEntity(Entity* entity);

	HierarchyDisplayer(bool spawnOpened = false, EditorManager* manager = nullptr) : 
	Panel(spawnOpened),m_manager(manager) {}
};