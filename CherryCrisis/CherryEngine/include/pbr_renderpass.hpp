#pragma once

#include <unordered_set>

#include <glad/gl.h>

#include "rendering_renderpass_interface.hpp"

#include "ebo_tbn_generator.hpp"
#include "texture_generator.hpp"
#include "element_mesh_generator.hpp"

#include "texture.hpp"
#include "light.hpp"

enum class ETextureType : unsigned int;
class ModelRenderer;
class ShapeRenderer;
class Material;
class Viewer;
class SkyRenderer;

class PBRRenderPass : public ARenderingRenderPass, ElementMeshGenerator
{
	ElementTBNGenerator m_meshGenerator;
	TextureGenerator m_textureGenerator;

	std::unordered_set<ModelRenderer*> m_models;
	std::unordered_set<ShapeRenderer*> m_shapes;
	std::unordered_set<Light*> m_lights;

	std::map<ETextureType, std::shared_ptr<Texture>> m_defaultTextures;

	SkyRenderer* m_skyRenderer = nullptr;

protected:
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
	void Unsubscribe(Light* toGenerate);

	template <>
	int Subscribe(ModelRenderer* toGenerate);

	template <>
	void Unsubscribe(ModelRenderer* toGenerate);

	template <>
	int Subscribe(ShapeRenderer* toGenerate);

	template <>
	void Unsubscribe(ShapeRenderer* toGenerate);

	template <>
	int Subscribe(SkyRenderer* toGenerate);

	template <>
	void Unsubscribe(SkyRenderer* toGenerate);

	void Generate(Material* toGenerate);

	void Execute(Framebuffer& framebuffer, Viewer*& viewer);
};
