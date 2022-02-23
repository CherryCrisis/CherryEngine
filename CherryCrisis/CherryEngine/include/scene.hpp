#pragma once

#include <vector>

#include "engine.h"

#include "resource.hpp"
#include "entity.hpp"
#include "skybox.hpp"

class CCENGINE_API Scene : public Resource
{
private:
	Scene(const char* filePath) : Resource(filePath) { }

public:
	Skybox m_skybox;

	static Scene* Create(const char* filePath);

	std::vector<Entity> m_entities;

	void Draw();
};