#pragma once

#include "renderer.hpp"
#include "viewer.hpp"
#include "cell.hpp"

#include "maths.hpp"

class ARenderingPipeline;

struct GPUPortal { };

struct Portal : public Viewer, public ARenderer
{
	std::unique_ptr<GPUPortal> m_gpuPortal;
	Portal* m_linkedPortal = nullptr;

	CCMaths::Matrix4 m_modelMatrix = CCMaths::Matrix4::Identity;
	CCMaths::Matrix4 m_viewLinkedPortal = CCMaths::Matrix4::Identity;

	float m_near = 0.01f;
	float m_far = 1500.f;
	float m_fovY = CCMaths::PI / 3.f;

	void Link(Portal* toLinkTo);

	void SubscribeToPipeline(ARenderingPipeline* pipeline) override;
	void UnsubscribeToPipeline(ARenderingPipeline* pipeline) override;
};