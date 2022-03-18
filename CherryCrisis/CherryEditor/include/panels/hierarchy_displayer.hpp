#pragma once

#include "core/panel.hpp"

class Scene;

class HierarchyDisplayer : public Panel 
{
private:
	Scene* m_displayedScene = nullptr;

	bool m_deleting = false;
	bool m_renaming = false;

public:
	void Render() override;
	void ContextCallback() override;

	void SetScene(Scene* scene) { m_displayedScene = scene; }

	void* m_focusedEntity = nullptr;
};