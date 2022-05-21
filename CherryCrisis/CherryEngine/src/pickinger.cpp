#include <pch.hpp>

#include "pickinger.hpp"

#include "entity.hpp"
#include "framebuffer.hpp"
#include "picking_renderpass.hpp"
#include "render_manager.hpp"
#include "scene_manager.hpp"
#include "ui_item.hpp"
#include "viewer.hpp"


template <>
Pickinger* Singleton<Pickinger>::currentInstance = nullptr;

uint32_t ColorToID(unsigned char color[3]) 
{
		return 
		color[0] +
		color[1] * 256 +
		color[2] * 256 * 256;
}

Entity* Pickinger::GetEntity(float x, float y) 
{
	Pickinger* instance = GetInstance();

	// flip texture
	y = (float)instance->m_viewer->m_framebuffer->height - y;

	instance->m_renderpass->Execute(instance->m_viewer);

	glFlush();
	glFinish();
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	unsigned char data[3];
	glReadPixels((int)x, (int)y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, data);

	return SceneManager::GetInstance()->m_currentScene->FindModelEntity(ColorToID(data));
}


Entity* Pickinger::GetEntity(const CCMaths::Vector2& position)
{
	return GetEntity(position.x, position.y);
}

void Pickinger::SetBuffer(Viewer* camera) 
{
	Pickinger* instance = GetInstance();
	
	instance->m_viewer = camera;
	instance->m_renderpass = camera->m_pipeline->LoadSubpipeline<PickingRenderPass>();
}

UIItem* Pickinger::GetUIItem(float x, float y)
{
	Pickinger* instance = GetInstance();

	// flip texture
	y = (float)instance->m_viewer->m_framebuffer->height - y;

	instance->m_renderpass->Execute(instance->m_viewer);

	glFlush();
	glFinish();
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	unsigned char data[3];
	glReadPixels((int)x, (int)y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, data);

	return SceneManager::GetInstance()->m_currentScene->m_UIContext.Get(ColorToID(data));
}


UIItem* Pickinger::GetUIItem(const CCMaths::Vector2& position)
{
	return GetUIItem(position.x, position.y);
}