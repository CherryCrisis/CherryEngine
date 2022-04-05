#include "pch.hpp"

#include "csscripting_system.hpp"

#include "monowrapper.hpp"
#include "csassembly.hpp"
#include "scripted_behaviour.hpp"

#include "utils.hpp"

template <>
CsScriptingSystem* Singleton<CsScriptingSystem>::currentInstance = nullptr;


CsScriptingSystem::CsScriptingSystem() = default;
CsScriptingSystem::~CsScriptingSystem() = default;

void CsScriptingSystem::Init()
{
	// TODO: Set generic pathes
	mono_set_dirs("lib", "externals\\etc");

	mono::ManagedScriptSystemSettings_t settings("RootDomain");
	m_scriptSystem = std::make_unique<mono::ManagedScriptSystem>(settings);

	MonoObject* (*GetStaticInstance)(ScriptedBehaviour*) = [](ScriptedBehaviour* managedPtr)
	{
		return managedPtr->GetRawInstance();
	};

	mono_add_internal_call("CCEngine.ScriptedBehaviour::GetStaticInstance", GetStaticInstance);
}

mono::ManagedScriptContext* CsScriptingSystem::CreateContext(char* domainName, const char* contextPath)
{
	auto newPath = CopyTemporaryFile(contextPath);

	return m_scriptSystem->CreateContext(domainName, contextPath, newPath.generic_string().c_str());
}

void CsScriptingSystem::InitializeAssembly(std::shared_ptr<CsAssembly> assembly, const char* domainName)
{
	char* name = (char*)&domainName[0];
	assembly->m_context = CreateContext(name, assembly->GetFilepath());

	m_assemblies.push_back(assembly);
}

void CsScriptingSystem::ReloadContextes()
{
	for (auto& assembly : m_assemblies)
		Resource<CsAssembly>::ReloadResource(assembly);
}
