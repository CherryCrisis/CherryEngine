#pragma once

#include "postprocess_renderpass_interface.hpp"
#include "element_mesh_generator.hpp"

#include "framebuffer.hpp"

class Mesh;

class HDRRenderPass : public APostProcessRenderPass
{
    ElementMeshGenerator m_meshGenerator;

    Framebuffer m_framebuffer;
    std::shared_ptr<Mesh> m_quadMesh;

public:
    TextureDisplayer* inBrightness = nullptr;

    HDRRenderPass(const char* name);

    void Execute(Framebuffer& framebuffer);
};