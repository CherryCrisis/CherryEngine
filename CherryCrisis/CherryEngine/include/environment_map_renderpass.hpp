#pragma once

#include <glad/gl.h>

#include "element_mesh_generator.hpp"
#include "rendering_renderpass_interface.hpp"
#include "texture.hpp"

class SkyRenderer;
class Viewer;


class EnvironmentMapRenderPass : public ARenderingRenderPass, ElementMeshGenerator
{
private:
	SkyRenderer* m_skyRenderer = nullptr;

	void SetupEnvironmentMap();
	void GenerateEnvironmentMap();

public:
	struct GPUEnvironmentMap: GPUTextureSpheremap
	{
		Event<std::shared_ptr<Texture>>* m_OnTextureReloaded = nullptr;
		std::shared_ptr<CCCallback::ACallback<>> m_OnGpuReloaded = nullptr;

		const int envMapSize = 1024;

		GLuint ID = 0u;

		void Generate(Texture* texture);
		void Regenerate(Texture* texture);
		void Destroy();

		GPUEnvironmentMap(Texture* texture);
		virtual ~GPUEnvironmentMap();
		void OnReload(std::shared_ptr<Texture> texture);
	};

	EnvironmentMapRenderPass(const char* name);

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
