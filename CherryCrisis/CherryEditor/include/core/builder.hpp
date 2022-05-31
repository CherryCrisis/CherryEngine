#pragma once 

#include <string>

class AssetBrowser;

class Builder 
{
public:
	static bool BuildGame(const char* outDir = "", const char* gameName="MyCherryGame", bool runGame = false, AssetBrowser* browser = nullptr);

	static bool CopyAssets(const std::string& path);
	static bool CopyCache(const std::string& path);
};