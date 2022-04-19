#pragma once

#include "postprocess_renderpass_interface.hpp"
#include "element_mesh_generator.hpp"

#include "framebuffer.hpp"

class Mesh;

class BasicPostProcessRenderPass : public APostProcessRenderPass, public ElementMeshGenerator
{
	Framebuffer m_framebuffer;

    enum class EColorTransform : GLuint
    {
        IDENTITY = 0u,
        GRAYSCALE = 1u,
        INVERSE = 2u,
        EDGEDETECTION = 3u,
        SEPIA = 4u,
    } transformType = EColorTransform::SEPIA;

    std::shared_ptr<Mesh> m_quadMesh;

public:
	BasicPostProcessRenderPass(const char* name);

	void Execute(Framebuffer& frambuffer);
};