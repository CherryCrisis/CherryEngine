#pragma once

#include "core/panel.hpp"

class Scene;
class EditorManager;
class Entity;

class HierarchyDisplayer : public Panel 
{
private:
	Scene* m_displayedScene = nullptr;
	EditorManager* m_manager = nullptr;

	bool m_deleting = false;
	bool m_renaming = false;

public:
	void Render() override;
	void ContextCallback() override;

	void SetScene(Scene* scene) { m_displayedScene = scene; }
	void RenderEntity(Entity* entity);


	HierarchyDisplayer(bool spawnOpened = false, EditorManager* manager = nullptr) : 
	Panel(spawnOpened),m_manager(manager) {}
};