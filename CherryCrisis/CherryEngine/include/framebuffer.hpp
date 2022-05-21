#pragma once

#include <glad/gl.h>

#include <cherry_macros.hpp>

class Camera;


struct CCENGINE_API TextureDisplayer
{
	GLuint texID = 0u;
};

struct CCENGINE_API Framebuffer
{
private:
	bool isInit = false;

	void UpdateTextureSize(float width, float height);

public :
	GLsizei height = 1920u, width = 1080u;

	GLuint FBO = 0u, RBO = 0u;
	TextureDisplayer colorTex;
	TextureDisplayer brightnessTex;

	Framebuffer() = default;
	~Framebuffer();

	void Init(float width = 1920, float height = 1080);

	//Verif if width and height are changed
	void UpdateFramebuffer(float width, float height);
};