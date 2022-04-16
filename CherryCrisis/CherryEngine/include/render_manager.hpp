#pragma once

#include "cherry_macros.hpp"

#include <unordered_map>
#include <functional>
#include <memory>

#include "singleton.hpp"

#include "rendering_renderpass_interface.hpp"
#include "postprocess_renderpass_interface.hpp"

class CCENGINE_API RenderManager : public Singleton<RenderManager>
{
	friend class Singleton<RenderManager>;

public:
	using RenderingRPDesc = std::function<void(std::vector<ARenderingRenderPass*>&)>;
	using PostprocessRPDesc = std::function<void(std::vector<APostProcessRenderPass*>&)>;

private:
	std::unordered_map<std::type_index, ARenderPass*>	m_existingRenderpasses;
	std::vector<ARenderingRenderPass*> m_orderedRenderingRenderpass;
	std::vector<APostProcessRenderPass*> m_orderedPostprocessRenderpass;

public:
	RenderManager();

	template <class SubPipelineT, class RendererT>
	void SubscribeToPipeline(RendererT* renderer);

	template <class SubPipelineT, class RendererT>
	void UnsubscribeToPipeline(RendererT* renderer);

	static void DrawScene(Framebuffer& framebuffer, Camera& camera);

	static RenderingRPDesc DefaultRenderingRenderpass();
	static PostprocessRPDesc DefaultPostprocessRenderpass();

	static void InitializePipeline(const RenderingRPDesc& renderingRenderpasses, const PostprocessRPDesc& postprocessRenderpasses);
	
	template <class SubPipelineT>
	constexpr SubPipelineT* GetSubpipeline();

	template <class SubPipelineT>
	constexpr SubPipelineT* LoadSubpipeline();
};

#include "renderer_manager.inl"