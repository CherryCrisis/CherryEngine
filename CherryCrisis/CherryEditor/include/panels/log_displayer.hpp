#pragma once

#include "core/panel.hpp"

class Debug;

class LogDisplayer : public Panel 
{
public:


	void Render() override;
	void TryClearOnPlay();
	
	LogDisplayer();

private:
	int m_showedLogMask;

	void RenderMenuBar();

	void Clear();

	Debug* m_debug = nullptr;

	bool m_isClearOnPlay    = true;
	bool m_isAutoScrolling  = true;
	bool m_isCollapsing		= true;

	bool m_displayWarning	= true;
	bool m_displayInfo		= true;
	bool m_displayError		= true;


	bool m_isScrollingTop = false;
	bool m_isScrollingBot = false;
};