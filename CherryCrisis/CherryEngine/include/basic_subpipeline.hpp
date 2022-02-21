#pragma once

#include <unordered_set>

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
	std::unordered_set<ModelRenderer*>	m_modelRenderers;

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

	template <>
	void Generate(ModelRenderer* toGenerate);

	void Execute() override;
};