#pragma once

#include <glad/gl.h>

#include "element_mesh_pipeline.hpp"

#include "skybox.hpp"

class SkyboxSubPipeline : public ElementMeshPipeline
{
private:
	Skybox* m_skybox = nullptr;

public:
	SkyboxSubPipeline(const char* name);

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

	void Execute() override;
};