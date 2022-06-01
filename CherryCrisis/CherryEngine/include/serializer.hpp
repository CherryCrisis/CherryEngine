#pragma once

#include <cherry_macros.hpp>

#include "scene.hpp"

class Behaviour;
class UIItem;

struct ProjectSettingsPack 
{
	std::string gameName;
	std::string gameVersion;
	std::string gameCompany;
};

struct BuilderPack
{
	std::string outDir;
};

struct EditorPack
{
	ProjectSettingsPack ps;
	BuilderPack		    builder;
};

class CCENGINE_API Serializer
{
private:
	static UIItem* CreateUIItem(const std::string& type, uint32_t uuid);
public:	
	static Behaviour* CreateBehaviour(const std::string& type, uint32_t uuid, bool hasUUID=true);
	// Save scene
	static bool SerializeScene(Scene* scene, const char* filepath ="");
	// Load scene
	static bool UnserializeScene(std::shared_ptr<Scene> scene, const char* filepath="");

	// Save editor user things 
	static bool SerializeEditor(EditorPack pack, const char* filepath = "");
	// Load editor user things 
	static EditorPack UnserializeEditor(const char* filepath = "");

	// Save game input context
	static bool SerializeInputs();
	// Load game input context
	static bool UnserializeInputs();

	// Save game related info for build
	static bool SerializeGame(const char* filepath);
	// Load game related info for build
	static bool UnserializeGame(const char* filepath);
};
