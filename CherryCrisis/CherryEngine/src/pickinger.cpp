#include "pch.hpp"

#include "pickinger.hpp"

#include "entity.hpp"
#include "framebuffer.hpp"
#include "picking_renderpass.hpp"
#include "render_manager.hpp"
#include "scene_manager.hpp"

template <>
Pickinger* Singleton<Pickinger>::currentInstance = nullptr;

Entity* Pickinger::GetEntity(float x, float y) 
{
	Pickinger* instance = GetInstance();

	// flip texture
	y = (float)instance->m_fbo->height - y;

	instance->m_renderpass->Execute(*instance->m_fbo, *instance->m_camera);

	glFlush();
	glFinish();
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	unsigned char data[3];
	glReadPixels(x, y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, data);
	
	uint32_t pickedID =
		data[0] +
		data[1] * 256 +
		data[2] * 256 * 256;

	return SceneManager::GetInstance()->m_currentScene->FindModelEntity(pickedID);
}


Entity* Pickinger::GetEntity(const CCMaths::Vector2& position)
{
	return GetEntity(position.x, position.y);
}

void Pickinger::SetBuffer(Framebuffer* buffer, Camera* camera) 
{
	Pickinger* instance = GetInstance();
	
	instance->m_fbo = buffer; instance->m_camera = camera;
	instance->m_renderpass = RenderManager::GetInstance()->LoadSubpipeline<PickingRenderPass>();
}