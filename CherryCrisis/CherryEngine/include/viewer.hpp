#pragma once

#include <memory>

#include "rendering_pipeline_interface.hpp"
#include "render_manager.hpp"

#include "maths.hpp"

#include "property.hpp"

class Cell;

class CCENGINE_API Viewer
{
public:
	Cell* m_ownerCell = nullptr;

	CCMaths::Matrix4 m_viewMatrix = CCMaths::Matrix4::Identity;
	CCMaths::Matrix4 m_projectionMatrix = CCMaths::Matrix4::Identity;

	std::unique_ptr<ARenderingPipeline> m_pipeline;

	Viewer() { RenderManager::GetInstance()->m_viewers.insert(this); }
	virtual ~Viewer() { RenderManager::GetInstance()->m_viewers.erase(this);  }
};