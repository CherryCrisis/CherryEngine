#pragma once

#include "core/panel.hpp"

#include "framebuffer.hpp"

class InputManager;
struct Camera;
class Scene;

class FrameDisplayer : public Panel 
{
private:
	bool m_isInit   = false;


	void Init();
	void UpdateTextureSize(float x, float y);

protected:
	Framebuffer m_framebuffer;
	
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
