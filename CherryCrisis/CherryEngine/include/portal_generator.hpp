#pragma once

#include <glad/gl.h>

#include "framebuffer.hpp"

#include "portal.hpp"

class CCENGINE_API PortalGenerator
{
public:
	struct GPUBasicPortal : GPUPortal
	{
		Framebuffer framebuffer;
	};

	virtual bool Generate(Portal* toGenerate);
};