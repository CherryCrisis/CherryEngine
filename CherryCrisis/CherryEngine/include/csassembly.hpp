#pragma once

#include "resource.hpp"

namespace mono
{
	class ManagedScriptContext;
}

class CsAssembly : public Resource<CsAssembly>
{
public:
	CsAssembly(const char* filePath) : Resource(filePath) { }

	std::shared_ptr<mono::ManagedScriptContext> context;

	static void Load(std::shared_ptr<CsAssembly> csassembly, const char* domainName);
};