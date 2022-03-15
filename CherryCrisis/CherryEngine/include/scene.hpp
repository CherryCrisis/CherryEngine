#pragma once

#include <vector>

#include "engine.hpp"

#include "resource.hpp"
#include "entity.hpp"
#include "skybox.hpp"

class ModelBase;

class CCENGINE_API Scene : public Resource
{
private:

public:
	Scene(const char* filePath) : Resource(filePath) { }

	Skybox m_skybox;

	static void Load(Ref<Scene> scene, const char* filePath);

	void GenerateEntities(std::shared_ptr<Resource> modelBase);

	std::vector<Entity> m_entities;

	void Draw();
};