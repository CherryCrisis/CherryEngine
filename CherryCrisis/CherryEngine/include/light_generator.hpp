#pragma once

#include <glad/gl.h>

#include "light.hpp"
#include "framebuffer.hpp"

class CCENGINE_API LightGenerator
{
public:
	struct GPULightBasic : public GPULight
	{
		Light* m_owner = nullptr;

		unsigned int index = 0u;

		GLuint FBO = 0u;
		GLuint TexID = 0u;
		GLuint width = 1000u;
		GLuint height = 1000u;

		GPULightBasic(Light* owner);
		~GPULightBasic();

		void Update(unsigned int offset, size_t size, void* data);

		static GLsizei ownerSize;
	};

	unsigned int m_lastIndex = 0;

	LightGenerator();
	~LightGenerator();

	virtual bool Generate(Light* toGenerate);

	static GLuint UBO;
	static GLuint UBOBindingPoint;
};