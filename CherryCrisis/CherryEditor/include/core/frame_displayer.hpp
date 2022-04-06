#pragma once

#include "core/panel.hpp"

class Camera;
class Scene;

class FrameDisplayer : public Panel 
{

private:
	bool m_isInit   = false;

	unsigned int m_FBO     = 0u;
	unsigned int m_RBO     = 0u;

	float m_width  = 0;
	float m_height = 0;

	void Init();
	void UpdateTextureSize(float x, float y);

protected:

	bool m_isActive = false;
	unsigned int m_ViewTex = 0u;

	void UpdateFramebuffer(float width, float height, Camera& camera);
	void Render() override;
};
