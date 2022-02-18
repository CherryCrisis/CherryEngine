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

	m_program = ResourceManager::GetInstance()->AddResource<ShaderProgram>("basicShader", true,
																		   "../Assets/basicShader.vert",
																		   "../Assets/basicShader.frag");
}

void RenderManager::DrawScene()
{
	RenderManager* RM = instance();

	glUseProgram(RM->m_program->m_shaderProgram);

	std::vector<ModelRenderer*> models;
	RM->GetAllRenderers<ModelRenderer>(models);
	
	for (ModelRenderer* model : models)
	{
		if (model->m_isVisible)
			model->Draw();
	}

	glUseProgram(0);
}
