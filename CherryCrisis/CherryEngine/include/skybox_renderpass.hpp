#pragma once

#include <glad/gl.h>

#include "rendering_renderpass_interface.hpp"

#include "element_mesh_generator.hpp"

#include "sky_renderer.hpp"

class Texture;
class Viewer;

class SkyboxRenderPass : public ARenderingRenderPass, ElementMeshGenerator
{
private:
	//ElementMeshGenerator m_meshGenerator;

	SkyRenderer* m_skyRenderer = nullptr;

	struct GPUSkyboxCubemap : public SkyRenderer::GPUSkybox
	{
		Event<std::shared_ptr<Texture>>* m_OnTextureReloaded = nullptr;
		std::shared_ptr<CCCallback::ACallback<>> m_OnGpuReloaded = nullptr;

		void Generate(Texture* texture);
		void Regenerate(Texture* texture);
		void Destroy();

		GPUSkyboxCubemap(Texture* texture);
		virtual ~GPUSkyboxCubemap();
		void OnReload(std::shared_ptr<Texture> texture);
	};

public:
	SkyboxRenderPass(const char* name);

	template <typename RendererT>
	int Subscribe(RendererT* toGenerate)
	{
		//static_assert(false, "RendererT generation is not implemented in BasicSubPipeline");
		return 1;
	}

	template <typename RendererT>
	void Unsubscribe(RendererT* toGenerate)
	{
		//static_assert(false, "RendererT deletion is not implemented in BasicSubPipeline");
	}

	template <>
	int Subscribe(SkyRenderer* toGenerate);

	template <>
	void Unsubscribe(SkyRenderer* toGenerate);

	void Execute(Viewer*& viewer);
};