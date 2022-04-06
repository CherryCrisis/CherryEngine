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

	mono::ManagedScriptContext* m_context = nullptr;

	static void Load(std::shared_ptr<CsAssembly> csassembly, const char* domainName);

	void Reload();
};