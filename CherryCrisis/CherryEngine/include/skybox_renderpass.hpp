#pragma once

#include <glad/gl.h>

#include "element_mesh_generator.hpp"

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
	int Subscribe(Skybox* toGenerate);

	template <>
	void Unsubscribe(Skybox* toGenerate);

	template <>
	int Subscribe(Camera* toGenerate);

	template <>
	void Unsubscribe(Camera* toGenerate);

	void Execute(Framebuffer& framebuffer, Camera& canera);
};