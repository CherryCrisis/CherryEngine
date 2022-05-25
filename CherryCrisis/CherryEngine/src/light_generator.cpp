#include "pch.hpp"

#include "light_generator.hpp"

GLuint LightGenerator::UBO = 0u;
GLuint LightGenerator::UBOBindingPoint = 0u;

GLsizei LightGenerator::GPULightBasic::ownerSize = offsetof(Light, alignement) - offsetof(Light, m_position) + sizeof(Light::alignement);

LightGenerator::GPULightBasic::GPULightBasic(Light* owner)
	: m_owner(owner)
{
	glGenTextures(1, &TexID);
	glBindTexture(GL_TEXTURE_2D, TexID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, TexID, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	m_owner->m_OnParamsChanged.Bind(&GPULightBasic::Update, this);
}

LightGenerator::GPULightBasic::~GPULightBasic()
{
	//m_owner->m_OnParamsChanged.Unbind(&GPULightBasic::Update, this);

	if (FBO)
		glDeleteFramebuffers(1, &FBO);

	if (TexID)
		glDeleteTextures(1, &TexID);

	glNamedBufferSubData(UBO, ownerSize * index, ownerSize, nullptr);
}

void LightGenerator::GPULightBasic::Update()
{
	Update(0, ownerSize, m_owner->m_position.data);
}

void LightGenerator::GPULightBasic::Update(unsigned int offset, size_t size, void* data)
{
	glNamedBufferSubData(UBO, ownerSize * index + offset, size, data);
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
	{
		glDeleteBuffers(1, &UBO);
		UBO = 0;
	}
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
	gpuLight->Update();

	return true;
}
