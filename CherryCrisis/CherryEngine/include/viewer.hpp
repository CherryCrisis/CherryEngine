#pragma once

#include <memory>

#include "rendering_pipeline_interface.hpp"
#include "render_manager.hpp"

#include "maths.hpp"

class Cell;

class Viewer
{
public:
	CCMaths::Matrix4 m_viewMatrix = CCMaths::Matrix4::Identity;
	CCMaths::Matrix4 m_projectionMatrix = CCMaths::Matrix4::Identity;

	std::unique_ptr<ARenderingPipeline> m_pipeline;

	Viewer() { RenderManager::GetInstance()->m_viewers.insert(this); }
	~Viewer() { RenderManager::GetInstance()->m_viewers.erase(this);  }
};