#pragma once

#include "cherry_macros.hpp"

class ARenderingPipeline;

class CCENGINE_API ARenderer
{
public:
	bool m_isVisible = true;

	virtual void SubscribeToPipeline(ARenderingPipeline* pipeline) { }
	virtual void UnsubscribeToPipeline(ARenderingPipeline* pipeline) { }

	virtual ~ARenderer() = default;
};