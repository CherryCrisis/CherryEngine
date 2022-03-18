#pragma once

#include <vector>

#include "engine.hpp"

#include "resource.hpp"
#include "entity.hpp"
#include "skybox.hpp"

class ModelBase;

class CCENGINE_API Scene : public Resource<Scene>
{
private:

public:
	Scene(const char* filePath) : Resource(filePath), m_int(std::make_shared<int>(5)) { }

	Skybox m_skybox;

	static void Load(Ref<Scene> scene, const char* filePath);

	void GenerateEntities(std::shared_ptr<ModelBase> modelBase);
	
	std::shared_ptr<int> m_int;
	Event<std::shared_ptr<int>> m_eventCallback;

	void GenerateEntitiesTest(std::shared_ptr<ModelBase> modelBase);

	std::vector<Entity> m_entities;

	void Draw();
};