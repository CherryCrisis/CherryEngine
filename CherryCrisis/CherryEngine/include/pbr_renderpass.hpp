#pragma once

#include <unordered_set>

#include <glad/gl.h>

#include "rendering_renderpass_interface.hpp"

#include "ebo_tbn_generator.hpp"

#include "texture.hpp"
#include "light.hpp"
#include "mesh.hpp"

class ModelRenderer;
class Material;
class Viewer;
class Skydome;

class PBRRenderPass : public ARenderingRenderPass, ElementTBNGenerator
{
	std::unordered_set<ModelRenderer*>	m_modelRenderers;
	std::unordered_set<Light*> m_lights;

	std::shared_ptr<Texture> m_defaultTexture;

	Skydome* m_skydome = nullptr;

protected:
	struct GPUTextureBasic : public GPUTexture
	{
		GLuint ID = 0u;

		void Generate(Texture* texture);
		void Regenerate(Texture* texture);
		void Destroy();

		GPUTextureBasic(Texture* texture);
		virtual ~GPUTextureBasic();
		void OnReload(std::shared_ptr<Texture> texture);
	};

	void BindTexture(Material* material, ETextureType textureType, int id);

public:
	PBRRenderPass(const char* name);

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
	int Subscribe(ModelRenderer* toGenerate);

	void Generate(Material* toGenerate);

	template <>
	int Subscribe(Texture* toGenerate);

	template <>
	int Subscribe(Skydome* toGenerate);

	template <>
	void Unsubscribe(Skydome* toGenerate);

	template <>
	void Unsubscribe(ModelRenderer* toGenerate);

	template <>
	void Unsubscribe(Light* toGenerate);

	void Execute(Framebuffer& framebuffer, Viewer*& viewer);
};
