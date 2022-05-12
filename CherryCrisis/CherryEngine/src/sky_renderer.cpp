#include "pch.hpp"

#include "sky_renderer.hpp"

#include "mesh.hpp"
#include "texture.hpp"

#include "environment_map_renderpass.hpp"
#include "irradiance_map_renderpass.hpp"
#include "prefilter_map_renderpass.hpp"
#include "brdf_renderpass.hpp"
#include "skydome_renderpass.hpp"
#include "render_manager.hpp"
#include "pbr_renderpass.hpp"

#include "skybox_renderpass.hpp"

#include "rendering_pipeline_interface.hpp"
#include "viewer.hpp"
#include "cell.hpp"

SkyRenderer::SkyRenderer(Cell* cell)
	: m_cell(cell)
{
	m_quad = ResourceManager::GetInstance()->AddResource<Mesh>("CC_normalizedQuad", true, EMeshShape::QUAD, 1.f, 1.f);
	m_quad->m_OnDeleted.Bind(&SkyRenderer::RemoveQuad, this);

	m_cube = ResourceManager::GetInstance()->AddResource<Mesh>("CC_normalizedCube", true, EMeshShape::CUBE, 0.5f, 0.5f, 0.5f);
	m_cube->m_OnDeleted.Bind(&SkyRenderer::RemoveCube, static_cast<SkyRenderer*>(this));
}

SkyRenderer::~SkyRenderer()
{
	RemoveCube();
	RemoveTexture();
	RemoveQuad();
}

void SkyRenderer::OnReloadTexture(std::shared_ptr<Texture> texture)
{
	texture->m_OnReloaded.Unbind(&SkyRenderer::OnReloadTexture, this);
	RemoveTexture();
	SetTexture(texture);
}

void SkyRenderer::OnSetTexture(std::shared_ptr<Texture> texture)
{
	texture->m_OnLoaded.Unbind(&SkyRenderer::OnSetTexture, this);
	SetTexture(texture);

}

void SkyRenderer::SetTexture(std::shared_ptr<Texture> texture)
{
	if (!texture)
	{
		RemoveTexture();
		return;
	}

	ETextureSurface textureSurface = texture->GetSurface();
	if (textureSurface == ETextureSurface::TEXTURE_2D)
		return;

	if (texture->GetResourceState() != EResourceState::LOADED)
	{
		texture->m_OnLoaded.Bind(&SkyRenderer::OnSetTexture, this);
		return;
	}

	RemoveTexture();

	texture->m_OnReloaded.Bind(&SkyRenderer::OnReloadTexture, this);
	m_texture = texture;

	m_cell->AddRenderer(this);
}

void SkyRenderer::RemoveCube()
{
	if (m_cube)
	{
		m_cube->m_OnDeleted.Unbind(&SkyRenderer::RemoveCube, this);
		m_cube = nullptr;

		// Move to function
		m_cell->RemoveRenderer(this);
	}
}

void SkyRenderer::RemoveQuad()
{
	if (m_quad)
	{
		m_quad->m_OnDeleted.Unbind(&SkyRenderer::RemoveQuad, this);
		m_quad = nullptr;

		// Move to function
		m_cell->RemoveRenderer(this);
	}
}


void SkyRenderer::RemoveTexture()
{
	if (m_texture)
	{
		m_texture->m_OnDeleted.Unbind(&SkyRenderer::RemoveTexture, this);
		m_texture->m_OnReloaded.Unbind(&SkyRenderer::OnReloadTexture, this);
		m_texture = nullptr;

		// Move to function
		m_cell->RemoveRenderer(this);
	}
}

void SkyRenderer::ClearData()
{
	m_cube->ClearData();
	m_quad->ClearData();
	m_texture->ClearData();
}

void SkyRenderer::SubscribeToPipeline(ARenderingPipeline* pipeline)
{
	if (m_texture->GetSurface() == ETextureSurface::TEXTURE_SPHEREMAP)
	{
		pipeline->SubscribeToPipeline<EnvironmentMapRenderPass>(this);
		pipeline->SubscribeToPipeline<IrradianceMapRenderPass>(this);
		pipeline->SubscribeToPipeline<PrefilterMapRenderPass>(this);
		pipeline->SubscribeToPipeline<BRDFRenderPass>(this);
		pipeline->SubscribeToPipeline<SkydomeRenderPass>(this);
		pipeline->SubscribeToPipeline<PBRRenderPass>(this);
	}
	else if (m_texture->GetSurface() == ETextureSurface::TEXTURE_CUBEMAP)
	{
		pipeline->SubscribeToPipeline<SkyboxRenderPass>(this);
	}
}

void SkyRenderer::UnsubscribeToPipeline(ARenderingPipeline* pipeline)
{
	pipeline->UnsubscribeToPipeline<SkyboxRenderPass>(this);
	pipeline->UnsubscribeToPipeline<EnvironmentMapRenderPass>(this);
	pipeline->UnsubscribeToPipeline<IrradianceMapRenderPass>(this);
	pipeline->UnsubscribeToPipeline<PrefilterMapRenderPass>(this);
	pipeline->UnsubscribeToPipeline<BRDFRenderPass>(this);
	pipeline->UnsubscribeToPipeline<SkydomeRenderPass>(this);
	pipeline->UnsubscribeToPipeline<PBRRenderPass>(this);
}

