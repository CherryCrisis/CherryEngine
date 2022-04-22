#include "pch.hpp"

#include "pickinger.hpp"

#include "entity.hpp"
#include "framebuffer.hpp"
#include "picking_renderpass.hpp"
#include "render_manager.hpp"
#include "scene_manager.hpp"

#include "viewer.hpp"

template <>
Pickinger* Singleton<Pickinger>::currentInstance = nullptr;

Entity* Pickinger::GetEntity(float x, float y) 
{
	Pickinger* instance = GetInstance();

	// flip texture
	y = (float)instance->m_fbo->colorTex.height - y;

	instance->m_renderpass->Execute(*instance->m_fbo, instance->m_viewer);

	glFlush();
	glFinish();
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	unsigned char data[3];
	glReadPixels((int)x, (int)y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, data);
	
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

void Pickinger::SetBuffer(Framebuffer* buffer, Viewer* camera) 
{
	Pickinger* instance = GetInstance();
	
	instance->m_fbo = buffer; instance->m_viewer = camera;
	instance->m_renderpass = camera->m_pipeline->LoadSubpipeline<PickingRenderPass>();
}