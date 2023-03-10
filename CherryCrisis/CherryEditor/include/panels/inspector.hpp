#pragma once

#include <unordered_map>
#include <functional>

#include "core/panel.hpp"

class AssetSettingsDisplayer;
class Behaviour;
class EditorManager;
class Engine;
class Entity;
class AssetSettingsDisplayer;
class Material;


class Inspector : public Panel 
{
private:
	struct ComponentInfos 
	{
		std::string type;
		std::string displayName; 
	};

	AssetSettingsDisplayer* m_assetSettingsDisplayer = nullptr;
	Behaviour*				m_copiedBehaviour = nullptr;
	EditorManager*			m_manager = nullptr;

	void InspectComponents(Entity* entity, int id);

	std::shared_ptr<Material> InspectMaterial(std::shared_ptr<Material> material);

	void DropzoneCallback(const std::string& path);
	void RenderAddComponentList();


	std::vector<ComponentInfos> m_componentsNames;
	
public:
	std::unordered_map<int, std::function<void(void*)>> m_inspects;

	Engine* m_engine = nullptr;

	void PopulateComponentList();
	Inspector(bool spawnOpened = true, EditorManager* manager = nullptr, AssetSettingsDisplayer* assetSettingsDisplayer = nullptr);

	void Render() override;
};
