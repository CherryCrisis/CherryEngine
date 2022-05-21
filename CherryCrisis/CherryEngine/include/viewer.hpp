#pragma once

#include <memory>

#include <glad/gl.h>

#include "framebuffer.hpp"
#include "frustum_plane.hpp"
#include "rendering_pipeline_interface.hpp"
#include "maths.hpp"

class Cell;


class CCENGINE_API Viewer
{
public:
	int m_currentIteration = 5;

	CCMaths::Matrix4 m_viewMatrix = CCMaths::Matrix4::Identity;
	CCMaths::Matrix4 m_projectionMatrix = CCMaths::Matrix4::Identity;
	
	CCMaths::Vector3 m_position = CCMaths::Vector3::Zero;

	FrustumPlanes m_frustumPlanes = {};

	std::unique_ptr<ARenderingPipeline> m_pipeline;

	std::shared_ptr<Framebuffer> m_framebuffer;
	
	Cell* m_ownerCell = nullptr;

	Viewer()
		: m_framebuffer(std::make_shared<Framebuffer>()) {}

	void Draw(int iteration)
	{
		glViewport(0, 0, m_framebuffer->width, m_framebuffer->height);

		// TODO: Make this parametrable
		//camera.Draw(m_framebuffer, 1);
		m_currentIteration = iteration;
		m_pipeline->Execute(this);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	virtual ~Viewer() = default;
};