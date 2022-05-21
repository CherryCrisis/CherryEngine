#pragma once

#include <unordered_map>
#include <functional>

#include "core/panel.hpp"

class AssetSettingsDisplayer;
class EditorManager;
class Engine;
class Entity;
class Material;

class Inspector : public Panel 
{
private:
	EditorManager* m_manager = nullptr;
	AssetSettingsDisplayer* m_assetSettingsDisplayer = nullptr;

	void InspectComponents(Entity* entity, int id);

	std::shared_ptr<Material> InspectMaterial(std::shared_ptr<Material> material);
public:

	Inspector(bool spawnOpened = true, EditorManager* manager = nullptr, AssetSettingsDisplayer* assetSettingsDisplayer = nullptr);

	void Render() override;
	
	std::unordered_map<int, std::function<void(void*)>> m_inspects;

	Engine* m_engine = nullptr;
};