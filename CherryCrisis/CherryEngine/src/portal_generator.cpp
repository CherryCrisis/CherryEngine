#include "pch.hpp"

#include "portal_generator.hpp"

bool PortalGenerator::Generate(Portal * toGenerate)
{
	if (!toGenerate)
		return false;

	if (toGenerate->m_gpuPortal)
		return true;

	auto gpuPortal = std::make_unique<GPUBasicPortal>();

	toGenerate->m_gpuPortal = std::move(gpuPortal);

	return true;
}
