#include "pch.hpp"

#include "portal_generator.hpp"

bool PortalGenerator::Generate(Portal * toGenerate)
{
	if (!toGenerate)
		return false;

	if (toGenerate->m_gpuPortal)
		return true;

	auto gpuPortal = std::make_unique<GPUBasicPortal>();

	gpuPortal->framebuffer.colorTex.width = 1000;
	gpuPortal->framebuffer.colorTex.height = 1000;

    Framebuffer& fb = gpuPortal->framebuffer;
    TextureDisplayer& colorTex = fb.colorTex;

    glGenFramebuffers(1, &fb.FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, fb.FBO);

    glGenTextures(1, &colorTex.texID);
    glBindTexture(GL_TEXTURE_2D, colorTex.texID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, colorTex.width, colorTex.height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex.texID, 0);

    TextureDisplayer& brightnessTex = fb.brightnessTex;

    glGenTextures(1, &brightnessTex.texID);
    glBindTexture(GL_TEXTURE_2D, brightnessTex.texID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, brightnessTex.width, brightnessTex.height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, brightnessTex.texID, 0);

    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);

    glGenRenderbuffers(1, &fb.RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, fb.RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, gpuPortal->framebuffer.colorTex.width, gpuPortal->framebuffer.colorTex.height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fb.RBO);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

	toGenerate->m_gpuPortal = std::move(gpuPortal);

	return true;
}
