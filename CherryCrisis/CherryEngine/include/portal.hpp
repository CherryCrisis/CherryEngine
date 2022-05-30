#pragma once

#include "cell.hpp"
#include "maths.hpp"
#include "renderer.hpp"
#include "viewer.hpp"

class	ARenderingPipeline;

struct	GPUPortal { };


struct Portal : public Viewer, public ARenderer
{
	float m_near = 0.0001f;
	float m_far = 1500.f;
	float m_fovY = CCMaths::PI / 3.f;

	CCMaths::Matrix4 m_modelMatrix = CCMaths::Matrix4::Identity;

	//portal matrix relative to linked portal
	CCMaths::Matrix4 m_relativeLinkedPortalMatrix = CCMaths::Matrix4::Identity;

	std::unique_ptr<GPUPortal> m_gpuPortal;

	Portal* m_linkedPortal = nullptr;

	void Link(Portal* toLinkTo);

	void SubscribeToPipeline(ARenderingPipeline* pipeline) override;
	void UnsubscribeToPipeline(ARenderingPipeline* pipeline) override;
};