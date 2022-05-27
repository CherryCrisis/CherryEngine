#pragma once

#include <map>
#include <unordered_set>

#include <cherry_macros.hpp>

#include <glad/gl.h>

#include "ebo_tbn_generator.hpp"
#include "light.hpp"
#include "rendering_renderpass_interface.hpp"
#include "texture_generator.hpp"
#include "light_generator.hpp"
#include "texture.hpp"

enum class ETextureType : unsigned int;
class Material;
class MeshRenderer;
class Viewer;


class CCENGINE_API BasicRenderPass : public ARenderingRenderPass
{
	ElementTBNGenerator m_meshGenerator;
	TextureGenerator	m_textureGenerator;
	LightGenerator		m_lightGenerator;

	std::unordered_set<MeshRenderer*>	m_models;
	std::vector<Light*>			m_lights;

	std::map<ETextureType, std::shared_ptr<Texture>> m_defaultTextures;

	void BindTexture(Material* material, ETextureType textureType, int id);

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
	void Unsubscribe(Light* toGenerate);

	template <>
	int Subscribe(MeshRenderer* toGenerate);

	template <>
	void Unsubscribe(MeshRenderer* toGenerate);

	void Generate(Material* toGenerate);

	void Execute(Viewer*& viewer);
};
