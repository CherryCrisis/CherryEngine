
#include "core/builder.hpp"
#include "command.hpp"
#include "serializer.hpp"
#include "utils.hpp"
#include "panels/asset_browser.hpp"

const std::set<std::string> keptExtensions = { ".ttf", ".wav", ".cs", ".cctexture", ".ccmat", ".ccscene"};

bool Builder::BuildGame(const char* outDir,const char* gameName, bool runGame, AssetBrowser* browser) 
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
	CopyAssets(path);
	CopyCache(path);

	//Then paste the exe & co and rename the exe et voila
	CopyFolder("Internal\\BuildKit", path);

	auto discard = rename(templatePath.c_str(), exePath.c_str());
	//Plus run the exe if runGame !
	if (runGame)
		call("open", exePath.c_str());

	return true;
}

bool Builder::CopyAssets(const std::string& path)
{
	auto directory_iterator = std::filesystem::directory_iterator("Assets");

	for (const auto& entry : directory_iterator)
	{
		std::string extension = entry.path().extension().string();

		if (keptExtensions.end() != keptExtensions.find(extension))
		{
			auto pathw = entry.path();
			CopyFile(pathw.string(), path+"\\"+ pathw.string());
		}
	}
	return true;
	//CopyFolder("Assets", path + "\\Assets");

	// loop on asset browser nodes
	// if directory and asset nodes are not empty, create directory then iterate on nodes
}

bool Builder::CopyCache(const std::string& path)
{
	CopyFolder("Cache", path + "\\Cache");

	//delete unused caches
	return true;
}