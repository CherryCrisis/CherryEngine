#include "pch.hpp"

#include "light_generator.hpp"

GLuint LightGenerator::UBO = 0u;
GLuint LightGenerator::UBOBindingPoint = 0u;

LightGenerator::LightGenerator()
{
	if (UBO == 0u)
	{
		int lightCount = 8;

		GLsizei uboSize = offsetof(Light, m_lightSpace) - offsetof(Light, m_position) + sizeof(Light::m_lightSpace);

		glGenBuffers(1, &UBO);
		glBindBuffer(GL_UNIFORM_BUFFER, UBO);
		glBufferData(GL_UNIFORM_BUFFER, 8 * uboSize, NULL, GL_STATIC_DRAW); // allocate 152 bytes of memory
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
}

bool LightGenerator::Generate(Light* toGenerate)
{
	if (!toGenerate)
		return false;

	if (toGenerate->m_gpuLight)
		return true;

	//toGenerate->m_gpuLight = std::make_unique<GPULightBasic>(toGenerate);

	
}
