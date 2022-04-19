#include <pch.hpp>

#include "builder.hpp"
#include "command.hpp"
#include "utils.hpp"
#include "serializer.hpp"

bool Builder::BuildGame(const char* outDir,const char* gameName, bool runGame) 
{
	//Create new File at outDir if it does not exists
	if (!CreateFolder(outDir, gameName))
		return false;

	//Then create a 'gamename'.master whereas is saved the main scene for example, or icon, gamename ect ect
	std::string gameNameStr = gameName;
	std::string path = std::string(outDir) + std::string("\\") + gameNameStr;
	std::string fullpath = path + "\\master";
	std::string templatePath = path + "\\" + "CherryStandalone.exe";
	std::string exePath = path +"\\"+gameName + ".exe";
	if (!Serializer::SerializeGame(fullpath.c_str()))
		return false;

	//Then paste the Assets folder by now, but clearly later paste only the used ones
	CopyFolder("Assets", path+"\\Assets");
	CopyFolder("Cache",  path +"\\Cache");
	//Then paste the exe & co and rename the exe et voila
	CopyFolder("Internal\\BuildKit", path);

	rename(templatePath.c_str(), exePath.c_str());
	//Plus run the exe if runGame !
	if (runGame)
		call("open", exePath.c_str());

	return true;
}