#pragma once

#include "singleton.hpp"
#include "maths/vector2.hpp"
#include "cherry_macros.hpp"

struct Framebuffer;
class Entity;
class UIItem;
class PickingRenderPass;
class Viewer;

class CCENGINE_API Pickinger : public Singleton<Pickinger>
{
	friend class Singleton<Pickinger>;

	PickingRenderPass*	m_renderpass = nullptr;
	Viewer*			    m_viewer = nullptr;
public:

	static Entity* GetEntity(float x, float y);
	static Entity* GetEntity(const CCMaths::Vector2& position);

	static UIItem* GetUIItem(float x, float y);
	static UIItem* GetUIItem(const CCMaths::Vector2& position);

	static void SetBuffer(Viewer* camera);
};