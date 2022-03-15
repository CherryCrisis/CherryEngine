#include "pch.hpp"

#include "csscripting_system.hpp"

template <>
CsScriptingSystem* Singleton<CsScriptingSystem>::currentInstance = nullptr;

CsScriptingSystem::CsScriptingSystem()
{
	mono_set_dirs("CherryEditor\\lib", "CherryEditor\\externals\\etc");
}

void CsScriptingSystem::Init(const mono::ManagedScriptSystemSettings_t& settings)
{
	m_scriptSystem = std::make_unique<mono::ManagedScriptSystem>(settings);
}