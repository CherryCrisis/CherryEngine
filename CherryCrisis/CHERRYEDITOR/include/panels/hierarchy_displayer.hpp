#pragma once

#include "core/panel.hpp"

class Scene;

class HierarchyDisplayer : public Panel 
{
private:
	Scene* m_displayedScene = nullptr;

public:
	void Render() override;

	void SetScene(Scene* scene) { m_displayedScene = scene; }
};