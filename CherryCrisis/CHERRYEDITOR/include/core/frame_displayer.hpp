#pragma once

#include "core/panel.hpp"

class Scene;

class FrameDisplayer : public Panel 
{

private:
	bool m_isInit   = false;

	unsigned int m_FBO     = 0u;
	unsigned int m_RBO     = 0u;

	void Init();

protected:

	bool m_isActive = false;
	unsigned int m_ViewTex = 0u;

	void UpdateFramebuffer(float width, float height);
	void Render() override;
};
