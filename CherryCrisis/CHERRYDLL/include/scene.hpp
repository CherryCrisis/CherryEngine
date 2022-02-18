#pragma once

#include <vector>

#include "engine.h"

#include "resource.hpp"
#include "entity.hpp"

class CHERRY_API Scene : public Resource
{
private:
	Scene(const char* filePath) : Resource(filePath) { }

public:
	static Scene* Create(const char* filePath);

	std::vector<Entity> m_entities;

	void Draw();
};