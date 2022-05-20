#pragma once

#include <unordered_set>
#include <map>

#include <glad/gl.h>

#include "rendering_renderpass_interface.hpp"

#include "ebo_tbn_generator.hpp"
#include "texture_generator.hpp"


#include <cherry_macros.hpp>

class Viewer;
class Collider;

class CCENGINE_API ColliderRenderPass : public ARenderingRenderPass
{
	ElementTBNGenerator m_meshGenerator;
	std::unordered_set<Collider*>	m_colliders;

	std::shared_ptr<Mesh> m_box = nullptr;
	std::shared_ptr<Mesh> m_sphere = nullptr;
	std::shared_ptr<Mesh> m_cylinder = nullptr;

public:
	ColliderRenderPass(const char* name);

	void DrawMesh(Mesh* mesh, const CCMaths::Matrix4& model);

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
	int Subscribe(Collider* toGenerate);

	template <>
	void Unsubscribe(Collider* toGenerate);

	void Execute(Viewer*& viewer);
};
