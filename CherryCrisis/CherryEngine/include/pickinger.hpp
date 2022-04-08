#pragma once

#include "singleton.hpp"
#include "maths/vector2.hpp"
#include "cherry_macros.hpp"

class Framebuffer;
class Entity;
class PickingRenderPass;
class Camera;

class CCENGINE_API Pickinger : public Singleton<Pickinger>
{
	friend class Singleton<Pickinger>;

	Framebuffer*			  m_fbo = nullptr;
	PickingRenderPass* m_renderpass = nullptr;
	Camera*			       m_camera = nullptr;
public:

	static Entity* GetEntity(float x, float y);
	static Entity* GetEntity(const CCMaths::Vector2& position);

	static void SetBuffer(Framebuffer* buffer, Camera* camera);
};