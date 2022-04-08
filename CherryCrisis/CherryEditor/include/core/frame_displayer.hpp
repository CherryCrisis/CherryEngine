#pragma once

#include "core/panel.hpp"

#include "framebuffer.hpp"

class InputManager;
class Camera;
class Scene;

class FrameDisplayer : public Panel 
{
private:
	bool m_isInit   = false;

	Framebuffer framebuffer;

	void Init();
	void UpdateTextureSize(float x, float y);

protected:
	InputManager* m_inputs;

	bool m_isActive = false;
	unsigned int m_ViewTex = 0u;

	void UpdateFramebuffer(float width, float height, Camera& camera);
	void Render() override;
public:
	bool m_isFocused = false;
	
	virtual void Focus() {}
	virtual void Unfocus() {}
};
