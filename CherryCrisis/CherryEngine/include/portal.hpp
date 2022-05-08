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

	void Link(Portal* toLinkTo);

	void SubscribeToPipeline(ARenderingPipeline* pipeline) override;
	void UnsubscribeToPipeline(ARenderingPipeline* pipeline) override;
};