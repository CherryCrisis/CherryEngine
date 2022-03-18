#pragma once

#include <glad/gl.h>

#include "element_mesh_renderpass.hpp"

#include "cubemap.hpp"

struct Skybox;
struct CameraComponent;

class SkyboxRenderPass : public ElementMeshRenderPass
{
private:
	Skybox* m_skybox = nullptr;
	CameraComponent* m_cameraComp = nullptr;

	struct GPUSkyboxCubemap : GPUCubemap
	{
		GLuint ID = 0u;
	};

public:
	SkyboxRenderPass(const char* name);

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
	int Generate(Skybox* toGenerate);

	template <>
	void Remove(Skybox* toGenerate);

	template <>
	int Generate(CameraComponent* toGenerate);

	template <>
	void Remove(CameraComponent* toGenerate);

	void Execute(const float x = 0, const float y = 0) override;
};