#pragma once

#include <unordered_set>

#include <glad/gl.h>

#include "element_mesh_generator.hpp"
#include "light.hpp"
#include "mesh.hpp"
#include "rendering_renderpass_interface.hpp"
#include "texture.hpp"

class MeshRenderer;
class UIItem;
class Viewer;


class PickingRenderPass : public ARenderingRenderPass
{
private:
	std::unordered_set<MeshRenderer*> m_models;
	std::unordered_set<UIItem*>	m_uiRenderers;

public:
	ElementMeshGenerator m_meshGenerator;

	struct GPUTextureBasic : GPUTexture2D
	{
		GLuint ID = 0u;
	};

	PickingRenderPass(const char* name);

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
	int Subscribe(MeshRenderer* toGenerate);

	template <>
	void Unsubscribe(MeshRenderer* toGenerate);

	template <>
	int Subscribe(UIItem* toGenerate);

	template <>
	void Unsubscribe(UIItem* toGenerate);

	void Execute(Viewer*& viewer);
};