#pragma once

#include <memory>

#include "rendering_pipeline_interface.hpp"

#include "maths.hpp"

class Cell;

class CCENGINE_API Viewer
{
public:
	int m_currentIteration = 5;
	Cell* m_ownerCell = nullptr;

	CCMaths::Matrix4 m_viewMatrix = CCMaths::Matrix4::Identity;
	CCMaths::Matrix4 m_projectionMatrix = CCMaths::Matrix4::Identity;
	CCMaths::Vector3 m_position;

	std::unique_ptr<ARenderingPipeline> m_pipeline;

	void Draw(Framebuffer& fb, int iteration)
	{
		m_currentIteration = iteration;
		m_pipeline->Execute(fb, this);
	}

	virtual ~Viewer() = default;
};