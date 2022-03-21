#include "pch.hpp"

#include "csassembly.hpp"
#include "csscripting_system.hpp"

#include "monowrapper.hpp"

void CsAssembly::Load(std::shared_ptr<CsAssembly> csassembly, char* domainName)
{
	csassembly->context = CsScriptingSystem::GetInstance()->CreateContext(domainName, csassembly->filepath.c_str());
}
