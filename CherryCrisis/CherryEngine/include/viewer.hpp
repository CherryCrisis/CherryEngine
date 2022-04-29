#pragma once

#include <memory>

#include "rendering_pipeline_interface.hpp"

#include "maths.hpp"

class Cell;

class CCENGINE_API Viewer
{
public:
	Cell* m_ownerCell = nullptr;

	CCMaths::Matrix4 m_viewMatrix = CCMaths::Matrix4::Identity;
	CCMaths::Matrix4 m_projectionMatrix = CCMaths::Matrix4::Identity;

	std::unique_ptr<ARenderingPipeline> m_pipeline;

	void Draw(Framebuffer& fb) { m_pipeline->Execute(fb, this); }

	virtual ~Viewer() = default;
};