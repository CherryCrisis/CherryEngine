#pragma once

#include <glad/gl.h>

#include "element_mesh_generator.hpp"
#include "rendering_renderpass_interface.hpp"
#include "texture.hpp"

class SkyRenderer;
class Viewer;


class IrradianceMapRenderPass : public ARenderingRenderPass, ElementMeshGenerator
{
private:

	SkyRenderer* m_skyRenderer	= nullptr;

	void SetupIrradianceMap();
	void GenerateIrradianceMap();

public:
	struct GPUIrradianceMapSphereMap : GPUIrradianceMap
	{
		Event<std::shared_ptr<Texture>>* m_OnTextureReloaded = nullptr;
		std::shared_ptr<CCCallback::ACallback<>> m_OnGpuReloaded = nullptr;

		GLuint		ID = 0u;
		const int	mapSize = 32;

		void Generate(Texture* texture);
		void Regenerate(Texture* texture);
		void Destroy();

		GPUIrradianceMapSphereMap(Texture* texture);
		virtual ~GPUIrradianceMapSphereMap();
		void OnReload(std::shared_ptr<Texture> texture);
	};

	IrradianceMapRenderPass(const char* name);

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
