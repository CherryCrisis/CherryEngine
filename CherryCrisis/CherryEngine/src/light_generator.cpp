#include "pch.hpp"

#include "light_generator.hpp"

GLuint LightGenerator::UBO = 0u;
GLuint LightGenerator::UBOBindingPoint = 0u;

GLsizei LightGenerator::GPULightBasic::ownerSize = offsetof(Light, m_outerCutoff) - offsetof(Light, m_position) + sizeof(Light::m_outerCutoff);

LightGenerator::GPULightBasic::GPULightBasic(Light* owner)
	: m_owner(owner)
{
	framebuffer.Init(1000, 1000);

	Update(0, ownerSize, m_owner->m_position.data);

	m_owner->m_OnParamsChanged.Bind(&GPULightBasic::Update, this);
}

LightGenerator::GPULightBasic::~GPULightBasic()
{
	m_owner->m_OnParamsChanged.Unbind(&GPULightBasic::Update, this);
}

void LightGenerator::GPULightBasic::Update(unsigned int offset, size_t size, void* data)
{
	glNamedBufferSubData(UBO, ownerSize * size_t(index) + offset, size, data);
}

LightGenerator::LightGenerator()
{
	if (!UBO)
	{
		size_t lightCount = 8u;

		glCreateBuffers(1, &UBO);
		glNamedBufferData(UBO, lightCount * size_t(LightGenerator::GPULightBasic::ownerSize), nullptr, GL_STATIC_DRAW);

		glBindBufferRange(GL_UNIFORM_BUFFER, UBOBindingPoint, UBO, 0, lightCount * size_t(LightGenerator::GPULightBasic::ownerSize));
	}
 }

LightGenerator::~LightGenerator()
{
	if (UBO)
		glDeleteBuffers(1, &UBO);
}

bool LightGenerator::Generate(Light* toGenerate)
{
	if (!toGenerate)
		return false;

	if (toGenerate->m_gpuLight)
		return true;

	toGenerate->m_gpuLight = std::make_unique<GPULightBasic>(toGenerate);
	auto gpuLight = static_cast<GPULightBasic*>(toGenerate->m_gpuLight.get());
	gpuLight->index = m_lastIndex++;

	return true;
}
