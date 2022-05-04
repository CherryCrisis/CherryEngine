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

	CCMaths::Vector3 m_position = CCMaths::Vector3::Zero;
	CCMaths::Vector3 m_rotation = CCMaths::Vector3::Zero;

	Portal* m_linkedPortal = nullptr;

	void Link(Portal* toLinkTo);

	void SubscribeToPipeline(ARenderingPipeline* pipeline) override;
	void UnsubscribeToPipeline(ARenderingPipeline* pipeline) override;
};