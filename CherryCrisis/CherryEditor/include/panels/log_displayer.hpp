#pragma once

#include <memory>

#include "core/panel.hpp"
#include "texture.hpp"

class Debug;
class Log;


struct GPUTextureLog : public GPUTexture2D
{
	unsigned int m_ID = 0;
};


class LogDisplayer : public Panel 
{
private:
	bool m_isClearOnPlay    = true;
	bool m_isAutoScrolling  = true;
	bool m_isCollapsing		= true;

	bool m_displayWarning	= true;
	bool m_displayInfo		= true;
	bool m_displayError		= true;

	int m_showedLogMask;
	
	std::shared_ptr<Texture> m_logTextures[3];

	Debug*	m_debug = nullptr;
	Log*	m_currentDisplayedLog = nullptr;
	void*	m_gpuTextureIDs[3];

	void GenerateGPUTexture(std::shared_ptr<Texture> texture);

	void RenderMenuBar();
	void Clear();

public:
	LogDisplayer();
	
	void Render() override;
	void TryClearOnPlay();
};