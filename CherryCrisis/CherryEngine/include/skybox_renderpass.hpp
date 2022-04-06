#pragma once

#include <glad/gl.h>

#include "element_mesh_renderpass.hpp"

#include "cubemap.hpp"

struct Skybox;
class Camera;

class SkyboxRenderPass : public ARenderPass, ElementMeshGenerator
{
private:
	Skybox* m_skybox = nullptr;
	Camera* m_camera = nullptr;

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
	int Generate(Camera* toGenerate);

	template <>
	void Remove(Camera* toGenerate);

	void Execute(const float& x, const float& y, Camera& canera);
};