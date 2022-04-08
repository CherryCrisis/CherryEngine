#pragma once

#include <glad/gl.h>

#include "cherry_macros.hpp"

struct CCENGINE_API Framebuffer
{
	GLuint FBO = 0u, RBO = 0u;
	GLsizei height = 1920u, width = 1080u;
};