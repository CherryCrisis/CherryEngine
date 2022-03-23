#include "pch.hpp"

#include "csassembly.hpp"
#include "csscripting_system.hpp"

#include "monowrapper.hpp"

void CsAssembly::Load(std::shared_ptr<CsAssembly> csassembly, const char* domainName)
{
	char* name = (char*)&domainName[0];
	csassembly->context = CsScriptingSystem::GetInstance()->CreateContext(name, csassembly->filepath.c_str());
}
