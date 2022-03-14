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
	Scene(const char* filePath) : Resource(filePath) { }

public:
	Skybox m_skybox;

	static Ref<Scene> Create(const char* filePath);

	void GenerateEntities(std::shared_ptr<Resource> modelBase);

	std::vector<Entity> m_entities;

	void Draw();
};