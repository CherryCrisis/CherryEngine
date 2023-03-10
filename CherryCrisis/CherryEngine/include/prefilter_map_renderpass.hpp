#pragma once

#include <glad/gl.h>

#include "element_mesh_generator.hpp"
#include "rendering_renderpass_interface.hpp"
#include "texture.hpp"

class SkyRenderer;
class Viewer;


class PrefilterMapRenderPass : public ARenderingRenderPass, ElementMeshGenerator
{
private:
	SkyRenderer* m_skyRenderer = nullptr;

	void SetupPrefilterMap();
	void GeneratePrefilterMap();

public:
	struct GPUPrefilterMapSphereMap : GPUPrefilterMap
	{
		Event<Texture*>* m_OnReloaded = nullptr;
		std::shared_ptr<CCCallback::ACallback<>> m_OnGpuReloaded = nullptr;

		const unsigned int	m_maxMipLevels = 5;
		const unsigned int	m_mipMapResolution = 128;

		GLuint		ID = 0u;

		void Generate(Texture* texture);
		void Regenerate(Texture* texture);
		void Destroy();

		GPUPrefilterMapSphereMap(Texture* texture);
		virtual ~GPUPrefilterMapSphereMap();
		void OnReload(Texture* texture);
	};

	PrefilterMapRenderPass(const char* name);

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
	int Subscribe(SkyRenderer* toGenerate);

	template <>
	void Unsubscribe(SkyRenderer* toGenerate);

};
