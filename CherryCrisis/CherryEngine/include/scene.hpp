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
	Scene(const char* filePath) 
		: Resource(filePath) { }

	Skybox m_skybox;

	static void Load(std::shared_ptr<Scene> scene);

	void GenerateEntities(std::shared_ptr<ModelBase> modelBase);

	std::vector<Entity> m_entities;

	void Draw();
};