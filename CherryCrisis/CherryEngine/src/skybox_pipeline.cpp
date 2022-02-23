#include "skybox_pipeline.hpp"

SkyboxSubPipeline::SkyboxSubPipeline(const char* name)
// TODO: Set real path
	: ElementMeshPipeline(name, "../Assets/skyboxShader.vert", "../Assets/skyboxShader.frag")
{

}

template <>
int SkyboxSubPipeline::Generate(Skybox* toGenerate)
{
	if (!toGenerate)
		return -1;

	Mesh* mesh = toGenerate->m_mesh.get();

	if (!mesh)
		return -1;

	if (ElementMeshPipeline::Generate(mesh) == -1)
		return -1;

	m_skybox = toGenerate;

	return 1;
}

template <>
void SkyboxSubPipeline::Remove(Skybox* toGenerate)
{
	if (m_skybox == toGenerate)
		m_skybox = nullptr;
}

void SkyboxSubPipeline::Execute()
{
	glUseProgram(m_program->m_shaderProgram);

	Mesh* mesh = m_skybox->m_mesh.get();

	GPUMeshBasic* gpuMesh = static_cast<GPUMeshBasic*>(mesh->m_gpuMesh);

	if (gpuMesh)
	{
		glBindVertexArray(gpuMesh->VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpuMesh->EBO);
		glDrawElements(GL_TRIANGLES, mesh->m_indices.size(), GL_UNSIGNED_INT, nullptr);
	}

	glUseProgram(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
