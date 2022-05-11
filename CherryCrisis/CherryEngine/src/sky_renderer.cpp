#include "pch.hpp"

#include "sky_renderer.hpp"

#include "mesh.hpp"

#include "rendering_pipeline_interface.hpp"
#include "viewer.hpp"
#include "cell.hpp"

SkyRenderer::SkyRenderer(Cell* cell)
	: m_cell(cell)
{

}

SkyRenderer::~SkyRenderer()
{
	RemoveCube();
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
