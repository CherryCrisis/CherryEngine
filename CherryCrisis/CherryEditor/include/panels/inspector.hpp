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
	AssetSettingsDisplayer* m_assetSettingsDisplayer = nullptr;
	EditorManager*			m_manager = nullptr;

	void InspectComponents(Entity* entity, int id);

	std::shared_ptr<Material> InspectMaterial(std::shared_ptr<Material> material);

public:
	std::unordered_map<int, std::function<void(void*)>> m_inspects;

	Engine* m_engine = nullptr;

	Inspector(bool spawnOpened = true, EditorManager* manager = nullptr, AssetSettingsDisplayer* assetSettingsDisplayer = nullptr);

	void Render() override;
};