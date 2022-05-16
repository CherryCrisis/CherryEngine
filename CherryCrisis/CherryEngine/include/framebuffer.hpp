#pragma once

#include <glad/gl.h>

#include "cherry_macros.hpp"

struct CCENGINE_API TextureDisplayer
{
	GLuint texID = 0u;
	GLsizei height = 1920u, width = 1080u;
};

struct CCENGINE_API Framebuffer
{
	GLuint FBO = 0u, RBO = 0u;
	TextureDisplayer colorTex;
	TextureDisplayer brightnessTex;

	~Framebuffer()
	{
		glDeleteBuffers(1, &FBO);
		glDeleteFramebuffers(1, &FBO);
		glDeleteRenderbuffers(1, &RBO);
		glDeleteBuffers(1, &colorTex.texID);
		glDeleteBuffers(1, &brightnessTex.texID);
	}
};