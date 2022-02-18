#include "render_manager.hpp"

#include <glad/gl.h>

#include "resourceManager.hpp"

#include "model_renderer.hpp"

template <>
RenderManager* Singleton<RenderManager>::currentInstance = nullptr;

RenderManager::RenderManager()
{
	if (gladLoaderLoadGL() == 0)
		printf("gladLoaderLoadGL failed\n");
}

void RenderManager::DrawScene()
{
	RenderManager* RM = instance();

	std::vector<ModelRenderer*> models;
	RM->GetAllRenderers<ModelRenderer>(models);
	
	for (ModelRenderer* model : models)
	{
		if (model->m_isVisible)
			model->Draw();
	}

	glUseProgram(0);
}
