#include "pch.hpp"

#include "picking_renderpass.hpp"

#include "model_renderer.hpp"
#include "transform.hpp"
#include "model.hpp"
#include "framebuffer.hpp"

#include "viewer.hpp"

PickingRenderPass::PickingRenderPass(const char* name)
	: ARenderingRenderPass(name, "Assets/picking.vert", "Assets/picking.frag")
{
	if (m_program)
		m_callExecute = CCCallback::BindCallback(&PickingRenderPass::Execute, this);
}

template <>
int PickingRenderPass::Subscribe(ModelRenderer* toGenerate)
{
	if (!toGenerate)
		return -1;

	m_modelRenderers.insert(toGenerate);

	return 1;
}

template <>
void PickingRenderPass::Unsubscribe(ModelRenderer* toGenerate)
{
	m_modelRenderers.erase(toGenerate);
}

CCMaths::Vector3 RGB(uint32_t ID)
{
	CCMaths::Vector3 Res = {};
	Res.b = ((ID & 0x00ff0000) >> 16) / 255.f;
	Res.g = ((ID & 0x0000ff00) >> 8) / 255.f;
	Res.r = ((ID & 0x000000ff) >> 0) / 255.f;
	return Res;
}

void PickingRenderPass::Execute(Framebuffer& fb, Viewer*& viewer)
{
	if (!viewer)
		return;

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glCullFace(GL_BACK);

	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_program->m_shaderProgram);

	CCMaths::Matrix4 viewProjection = viewer->m_projectionMatrix * viewer->m_viewMatrix;
	glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uViewProjection"), 1, GL_FALSE, viewProjection.data);

	for (ModelRenderer* modelRdr : m_modelRenderers)
	{
		CCMaths::Matrix4 modelMat = modelRdr->m_transform->GetWorldMatrix();
		glUniformMatrix4fv(glGetUniformLocation(m_program->m_shaderProgram, "uModel"), 1, GL_FALSE, modelMat.data);

		Model* model = modelRdr->m_model.get();

		if (!model || !model->m_mesh.get())
			continue;

		CCMaths::Vector3 colorID = RGB(modelRdr->m_id);
		glUniform4f(glGetUniformLocation(m_program->m_shaderProgram, "uColorID"), colorID.r, colorID.g, colorID.b, 1.f);

		Mesh* mesh = model->m_mesh.get();
		GPUMeshBasic* gpuMesh = static_cast<GPUMeshBasic*>(mesh->m_gpuMesh.get());

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