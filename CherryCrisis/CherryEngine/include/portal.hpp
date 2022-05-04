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

	CCMaths::Matrix4 m_modelMatrix = CCMaths::Matrix4::Identity;

	Portal* m_linkedPortal = nullptr;

	void Link(Portal* toLinkTo);

	void SubscribeToPipeline(ARenderingPipeline* pipeline) override;
	void UnsubscribeToPipeline(ARenderingPipeline* pipeline) override;
};