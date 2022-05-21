#pragma once

#include <unordered_set>

#include <glad/gl.h>

#include "ebo_tbn_generator.hpp"
#include "framebuffer.hpp"
#include "rendering_renderpass_interface.hpp"
#include "texture.hpp"

class AudioEmitter;
class CameraComponent;
class LightComponent;
class Viewer;


class GuizmoRenderPass : public ARenderingRenderPass
{
private:

	std::unordered_set<AudioEmitter*>	    m_audioEmitters;
	std::unordered_set<CameraComponent*>	m_cameraComponents;
	std::unordered_set<LightComponent*>	    m_lightComponents;

	std::shared_ptr<Mesh> m_quadMesh;

	std::shared_ptr<Texture> m_cameraIcon;
	std::shared_ptr<Texture> m_audioIcon;
	std::shared_ptr<Texture> m_lightIcon;

protected:
	ElementMeshGenerator m_meshGenerator;

public:
	GuizmoRenderPass(const char* name);

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
	int Subscribe(AudioEmitter* toGenerate);
	template <>
	void Unsubscribe(AudioEmitter* toGenerate);

	template <>
	int Subscribe(CameraComponent* toGenerate);
	template <>
	void Unsubscribe(CameraComponent* toGenerate);

	template <>
	int Subscribe(LightComponent* toGenerate);
	template <>
	void Unsubscribe(LightComponent* toGenerate);

	void Execute(Viewer*& viewer);
};