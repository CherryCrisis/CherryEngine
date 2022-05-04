#pragma once

#include "postprocess_renderpass_interface.hpp"
#include "element_mesh_generator.hpp"

#include "framebuffer.hpp"

class Mesh;

class BloomRenderPass : public APostProcessRenderPass
{
    ElementMeshGenerator m_meshGenerator;

    Framebuffer m_pingpongFramebuffers[2];
    std::shared_ptr<Mesh> m_quadMesh;

public:
    int m_iterations = 3;

    TextureDisplayer* outBrightness = nullptr;

    BloomRenderPass(const char* name);

    void Execute(Framebuffer& framebuffer);
};