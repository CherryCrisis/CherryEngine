#pragma once

#include <unordered_map>
#include <typeindex>
#include <typeinfo>
#include <memory>

#include "singleton.hpp"

#include "engine.h"

#include "renderer.hpp"
#include "subpipeline_interface.hpp"

#include "shaderProgram.hpp"

class CCENGINE_API RenderManager : public Singleton<RenderManager>
{
	friend class Singleton<RenderManager>;

private:
	std::unordered_map<std::type_index, ASubPipeline*>		m_subpipelines;

public:
	RenderManager();

	template <class SubPipelineT, class RendererT>
	void GenerateFromPipeline(RendererT* renderer);

	static void DrawScene();
};

#include "renderer_manager.inl"