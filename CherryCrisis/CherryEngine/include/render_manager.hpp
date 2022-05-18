#pragma once

#include "cherry_macros.hpp"

#include <set>
#include <memory>

#include "singleton.hpp"

struct Framebuffer;
class Viewer;

class CCENGINE_API RenderManager : public Singleton<RenderManager>
{
	friend class Singleton<RenderManager>;

public:
	RenderManager();
	~RenderManager();
};
