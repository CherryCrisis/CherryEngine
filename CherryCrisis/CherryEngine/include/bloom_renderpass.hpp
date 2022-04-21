#pragma once

#include "postprocess_renderpass_interface.hpp"
#include "element_mesh_generator.hpp"

#include "framebuffer.hpp"

class Mesh;

class BloomRenderPass : public APostProcessRenderPass, public ElementMeshGenerator
{
    Framebuffer m_pingpongFramebuffers[2];
    std::shared_ptr<Mesh> m_quadMesh;

    std::shared_ptr<ShaderProgram> m_hdrProgram;

public:
    int m_iterations = 3;

    BloomRenderPass(const char* name);

    void Execute(Framebuffer& frambuffer);
};