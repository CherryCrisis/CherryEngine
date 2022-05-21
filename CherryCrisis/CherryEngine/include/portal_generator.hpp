#pragma once

#include <glad/gl.h>

#include "framebuffer.hpp"
#include "portal.hpp"


class CCENGINE_API PortalGenerator
{
public:
	struct GPUBasicPortal : GPUPortal
	{
		std::shared_ptr<Framebuffer> framebuffer;

		GPUBasicPortal()
			: framebuffer(std::make_shared<Framebuffer>())
		{
			framebuffer->Init();
		}
	};

	virtual bool Generate(Portal* toGenerate);
};