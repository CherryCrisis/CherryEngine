#pragma once

#include <string>
#include <unordered_map>

#include "resource.hpp"
#include "entity.hpp"
#include "skybox.hpp"

class CCENGINE_API Scene : public Resource
{
private:
	Scene(const char* filePath) : Resource(filePath) { }

public:
	~Scene();

	Skybox m_skybox;

	// TODO: Switch to unique_ptr
	std::unordered_map<std::string, Entity*> m_entities;

	void Start();
	void Update();

	void Draw();

	std::string GetUniqueEntityName(const std::string& entityName);

	void AddEntity(Entity* toAdd);

	static Ref<Scene> Create(const char* filePath);

	bool Serialize(const char* filePath);
};