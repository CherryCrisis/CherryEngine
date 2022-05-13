#include "pch.hpp"

#include "ui_renderpass.hpp"

#include "framebuffer.hpp"

#include "camera.hpp"
#include "ui_image.hpp"
#include "viewer.hpp"

UIRenderPass::UIRenderPass(const char* name)
	: ARenderingRenderPass(name, "Assets/Shaders/uiShader.vert", "Assets/Shaders/uiShader.frag")
{
	if (m_program)
	{
		m_callExecute = CCCallback::BindCallback(&UIRenderPass::Execute, this);
	}
}

template <>
int UIRenderPass::Subscribe(UIImage* toGenerate)
{
	if (!toGenerate)
		return -1;

	m_uiImages.insert(toGenerate);

	return 1;
}

template <>
void UIRenderPass::Unsubscribe(UIImage* toGenerate)
{
	m_uiImages.erase(toGenerate);
}

void UIRenderPass::Execute(Framebuffer& framebuffer, Viewer*& viewer)
{
	if (!viewer)
		return;

	glViewport(0, 0, framebuffer.colorTex.width, framebuffer.colorTex.height);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.FBO);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LESS);

	glCullFace(GL_BACK);

	glUseProgram(m_program->m_shaderProgram);

	//CCMaths::Matrix4 viewProjection = viewer->m_projectionMatrix * viewer->m_viewMatrix;
	//glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uViewProjection"), 1, GL_FALSE, viewProjection.data);

	for (UIImage* image : m_uiImages)
	{
		CCMaths::Vector3 pos;
		image->Position.Get(&pos);
		pos.x /= framebuffer.colorTex.width;
		pos.y /= framebuffer.colorTex.height;
		
		CCMaths::Matrix4 modelMat = CCMaths::Matrix4::Translate(pos) * CCMaths::Matrix4::Scale({ .01f * image->GetSize().x, .01f * image->GetSize().y, 1.f });
		glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uModel"), 1, GL_FALSE, modelMat.data);

		auto gpuTexture = static_cast<TextureGenerator::GPUTextureBasic*>(image->m_texture->m_gpuTexture.get());
		glBindTextureUnit(0, gpuTexture->ID);

		Mesh* mesh = image->m_mesh.get();
		if (!mesh)
			continue;

		auto gpuMesh = static_cast<ElementTBNGenerator::GPUMeshBasic*>(mesh->m_gpuMesh.get());
		if (!gpuMesh)
			continue;

		glBindVertexArray(gpuMesh->VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpuMesh->EBO);
		glDrawElements(GL_TRIANGLES, gpuMesh->indicesCount, GL_UNSIGNED_INT, nullptr);
	}

	glUseProgram(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
