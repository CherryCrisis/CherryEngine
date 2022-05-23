#pragma once

#include <unordered_map>
#include <functional>

#include "core/panel.hpp"

class Engine;
class EditorManager;
class Entity;
class AssetSettingsDisplayer;
class Behaviour;

class Inspector : public Panel 
{
private:
	struct ComponentInfos 
	{
		std::string type;
		std::string displayName; 
	};

	Behaviour* copiedBehaviour = nullptr;

	EditorManager* m_manager = nullptr;
	AssetSettingsDisplayer* m_assetSettingsDisplayer = nullptr;

	void InspectComponents(Entity* entity, int id);

	void DropzoneCallback(const std::string& path);
	void RenderAddComponentList();

	void PopulateComponentList();

	std::vector<ComponentInfos> m_componentsNames;
public:

	Inspector(bool spawnOpened = true, EditorManager* manager = nullptr, AssetSettingsDisplayer* assetSettingsDisplayer = nullptr);

	void Render() override;
};