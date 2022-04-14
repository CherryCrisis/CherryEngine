#pragma once

#include <unordered_set>

#include <glad/gl.h>

#include "ebo_tbn_generator.hpp"

#include "texture.hpp"
#include "light.hpp"
#include "mesh.hpp"

#include "camera.hpp"

class ModelRenderer;
class Material;

class BasicRenderPass : public ARenderPass, ElementTBNGenerator
{
	std::unordered_set<ModelRenderer*>	m_modelRenderers;
	std::unordered_set<Light*> m_lights;
	
	Camera* m_camera = nullptr;

protected:
	struct GPUTextureBasic : public GPUTexture
	{
		GLuint ID = 0u;

		virtual ~GPUTextureBasic();
	};

public:
	BasicRenderPass(const char* name);

	template <typename RendererT>
	int Subscribe(RendererT* toGenerate)
	{
		static_assert(false, "RendererT generation is not implemented in BasicSubPipeline");
	}

	template <typename RendererT>
	void Unsubscribe(RendererT* toGenerate)
	{
		static_assert(false, "RendererT deletion is not implemented in BasicSubPipeline");
	}

	template <>
	int Subscribe(Light* toGenerate);

	template <>
	int Subscribe(Camera* toGenerate);

	template <>
	int Subscribe(ModelRenderer* toGenerate);

	template <>
	int Subscribe(Material* toGenerate);

	template <>
	int Subscribe(Texture* toGenerate);

	template <>
	void Unsubscribe(Camera* toGenerate);

	template <>
	void Unsubscribe(ModelRenderer* toGenerate);

	template <>
	void Unsubscribe(Light* toGenerate);

	void Execute(Framebuffer& framebuffer, Camera& camera);
};