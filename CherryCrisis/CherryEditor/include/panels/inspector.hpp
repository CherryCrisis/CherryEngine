#pragma once

#include <unordered_map>
#include <functional>

#include "core/panel.hpp"

class Engine;
class EditorManager;

class Inspector : public Panel 
{
private:
	EditorManager* m_manager = nullptr;
public:

	Inspector(bool spawnOpened = true, EditorManager* manager = nullptr);

	void Render() override;
	
	std::unordered_map<int, std::function<void(void*)>> m_inspects;

	Engine* m_engine = nullptr;
};