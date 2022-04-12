#pragma once

#include <cherry_macros.hpp>
#include "scene.hpp"

class CCENGINE_API Serializer
{
private:


public:	
	// Save scene
	static bool SerializeScene(Scene* scene);
	// Load scene
	static bool UnserializeScene(std::shared_ptr<Scene> scene);
};