#include <pch.hpp>

#include "csassembly.hpp"

#include "csscripting_system.hpp"
#include "monowrapper.hpp"
#include "utils.hpp"


void CsAssembly::Load(std::shared_ptr<CsAssembly> csassembly, const char* domainName, bool makeCopy)
{
	// TODO: try const_cast
	CsScriptingSystem::GetInstance()->InitializeAssembly(csassembly, domainName, makeCopy);
}

void CsAssembly::Reload()
{
	if (!m_context->Unload())
		return;

	CopyTemporaryFile(m_context->GetPath().c_str());

	m_context->Init();
}