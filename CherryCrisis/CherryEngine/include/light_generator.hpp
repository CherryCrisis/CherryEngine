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

		size_t index = 0u;

		GLuint FBO = 0u;
		GLuint TexID = 0u;
		GLuint width = 1000u;
		GLuint height = 1000u;

		GPULightBasic(Light* owner);
		~GPULightBasic();

		void Update();
		void Update(unsigned int offset, size_t size, void* data);
		void GetCorrectIndex();

		static GLsizei ownerSize;
		static std::set<unsigned int> indices;
	};

	LightGenerator();
	~LightGenerator();

	virtual bool Generate(Light* toGenerate);

	static GLuint UBO;
	static GLuint UBOBindingPoint;
};