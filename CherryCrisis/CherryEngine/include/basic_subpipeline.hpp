#pragma once

#include "subpipeline_interface.hpp"

#include "model_renderer.hpp"
#include "transform.hpp"
#include "texture.hpp"
#include "model.hpp"
#include "maths.hpp"
#include "mesh.hpp"

#include <glad/gl.h>

class BasicSubPipeline : public ASubPipeline
{
public:
	struct GPUMeshBasic : GPUMesh
	{
		GLuint VAO = 0u, VBO = 0u, EBO = 0u;
	};

	struct GPUTextureBasic : GPUTexture
	{
		GLuint ID;
	};

	BasicSubPipeline(const char* name);

	template <typename RendererT>
	void Generate(RendererT* toGenerate)
	{
		static_assert(false, "RendererT generation is not implemented in BasicSubPipeline");
	}

	template <typename RendererT>
	void Consume(RendererT* toGenerate)
	{
		static_assert(false, "RendererT consumption is not implemented in BasicSubPipeline");
	}

	template <>
	void Generate(ModelRenderer* toGenerate);

	template <>
	void Consume(ModelRenderer* toGenerate);
};