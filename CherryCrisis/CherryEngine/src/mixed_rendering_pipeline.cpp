#include "pch.hpp"

#include "mixed_rendering_pipeline.hpp"

#include "skydome_renderpass.hpp"
#include "basic_rendering_pipeline.hpp"
#include "pbr_rendering_pipeline.hpp"

MixedPipeline::MixedPipeline()
{
	m_skydomePass = LoadSubpipeline<SkydomeRenderPass>();
}

void MixedPipeline::Execute(Framebuffer& framebuffer, Viewer* viewer)
{
	
	m_skydomePass->CallOnExecute(framebuffer, viewer);
}