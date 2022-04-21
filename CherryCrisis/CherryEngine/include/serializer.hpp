#pragma once

#include <cherry_macros.hpp>
#include "scene.hpp"

class Behaviour;

class CCENGINE_API Serializer
{
private:
	static Behaviour* CreateBehaviour(const std::string& type, uint32_t uuid);
public:	
	// Save scene
	static bool SerializeScene(Scene* scene, const char* filepath ="");
	// Load scene
	static bool UnserializeScene(std::shared_ptr<Scene> scene, const char* filepath="");
	//Save editor user things 
	static bool SerializeEditor(const char* filepath);
	//Save game related info for build
	static bool SerializeGame(const char* filepath);

	static bool UnserializeGame(const char* filepath);

	
};