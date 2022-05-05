#pragma once

#include <unordered_map>
#include <functional>

#include "core/panel.hpp"

class Engine;
class EditorManager;
class Entity;
class AssetSettingsDisplayer;

class Inspector : public Panel 
{
private:
	EditorManager* m_manager = nullptr;
	AssetSettingsDisplayer* m_assetSettingsDisplayer = nullptr;

	void InspectComponents(Entity* entity, int id);
public:

	Inspector(bool spawnOpened = true, EditorManager* manager = nullptr, AssetSettingsDisplayer* assetSettingsDisplayer = nullptr);

	void Render() override;
	
	std::unordered_map<int, std::function<void(void*)>> m_inspects;

	Engine* m_engine = nullptr;
};