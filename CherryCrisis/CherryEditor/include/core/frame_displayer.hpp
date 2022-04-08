#pragma once

#include "core/panel.hpp"

class InputManager;
struct Camera;
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
	InputManager* m_inputs = nullptr;

	bool m_isActive = false;
	unsigned int m_ViewTex = 0u;

	void UpdateFramebuffer(float width, float height, Camera& camera);
	void Render() override;
public:
	bool m_isFocused = false;
	
	virtual void Focus() {}
	virtual void Unfocus() {}
};
