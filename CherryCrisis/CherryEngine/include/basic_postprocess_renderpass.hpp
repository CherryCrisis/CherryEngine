#pragma once

#include "element_mesh_generator.hpp"
#include "framebuffer.hpp"
#include "postprocess_renderpass_interface.hpp"

class Mesh;


class BasicPostProcessRenderPass : public APostProcessRenderPass 
{
    enum class EColorTransform : GLuint
    {
        IDENTITY = 0u,
        GRAYSCALE = 1u,
        INVERSE = 2u,
        EDGEDETECTION = 3u,
        SEPIA = 4u,
    } transformType = EColorTransform::SEPIA;

    ElementMeshGenerator m_meshGenerator;

	Framebuffer m_framebuffer;

    std::shared_ptr<Mesh> m_quadMesh;

public:
	BasicPostProcessRenderPass(const char* name);

	void Execute(Framebuffer& framebuffer);
};