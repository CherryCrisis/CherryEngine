#pragma once

#include <unordered_set>

#include <glad/gl.h>

#include "element_mesh_renderpass.hpp"

#include "texture.hpp"
#include "light.hpp"
#include "mesh.hpp"

#include "camera.hpp"

class ModelRenderer;
class Material;

class BasicRenderPass : public ARenderPass, ElementMeshGenerator
{
	std::unordered_set<ModelRenderer*>	m_modelRenderers;
	std::unordered_set<Light*> m_lights;
	
	Camera* m_camera = nullptr;

public:
	struct GPUTextureBasic : GPUTexture
	{
		GLuint ID = 0u;
	};

	BasicRenderPass(const char* name);

	template <typename RendererT>
	int Generate(RendererT* toGenerate)
	{
		static_assert(false, "RendererT generation is not implemented in BasicSubPipeline");
	}

	template <typename RendererT>
	void Remove(RendererT* toGenerate)
	{
		static_assert(false, "RendererT deletion is not implemented in BasicSubPipeline");
	}

	template <>
	int Generate(Light* toGenerate);

	template <>
	int Generate(Camera* toGenerate);

	template <>
	int Generate(ModelRenderer* toGenerate);

	template <>
	int Generate(Material* toGenerate);

	template <>
	int Generate(Texture* toGenerate);

	template <>
	void Remove(Camera* toGenerate);

	template <>
	void Remove(ModelRenderer* toGenerate);

	template <>
	void Remove(Light* toGenerate);

	void Execute(const float& x, const float& y, Camera& camera);
};