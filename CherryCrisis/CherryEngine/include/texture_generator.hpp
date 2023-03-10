#pragma once

#include <glad/gl.h>

#include "texture.hpp"


class CCENGINE_API TextureGenerator
{
public:
	struct CCENGINE_API GPUTextureBasic : public GPUTexture2D
	{
		GLuint ID = 0u;

		void Generate(Texture* texture);
		void Regenerate(Texture* texture);
		void Destroy();

		GPUTextureBasic(Texture* texture);
		virtual ~GPUTextureBasic();
		void OnReload(Texture* texture);
	};

	virtual bool Generate(Texture* toGenerate);
};