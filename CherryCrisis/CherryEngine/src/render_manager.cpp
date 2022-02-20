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
	
	glClearColor(1.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (ModelRenderer* model : models)
	{
		if (model->m_isVisible)
			model->Draw();
	}

	glUseProgram(0);
}
