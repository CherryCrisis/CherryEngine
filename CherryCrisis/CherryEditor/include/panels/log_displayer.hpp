#pragma once

#include <memory>

#include "core/panel.hpp"
#include "texture.hpp"

class Debug;
class Log;
//class Texture;

//struct GPUTexture;
struct GPUTextureLog : GPUTexture
{
	unsigned int m_ID;
};

class LogDisplayer : public Panel 
{
private:
	int m_showedLogMask;

	Debug* m_debug = nullptr;
	Log* m_currentDisplayedLog = nullptr;

	bool m_isClearOnPlay    = true;
	bool m_isAutoScrolling  = true;
	bool m_isCollapsing		= true;

	bool m_displayWarning	= true;
	bool m_displayInfo		= true;
	bool m_displayError		= true;

	bool m_isScrollingTop = false;
	bool m_isScrollingBot = false;

	std::shared_ptr<Texture> m_logTextures[3];

	void GenerateGPUTexture(std::shared_ptr<Texture> texture);

	void RenderMenuBar();
	void Clear();

public:


	void Render() override;
	void TryClearOnPlay();
	
	LogDisplayer();

};