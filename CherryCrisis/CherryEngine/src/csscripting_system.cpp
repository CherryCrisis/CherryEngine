#include "pch.hpp"

#include "csscripting_system.hpp"

#include <filesystem>

#include "monowrapper.hpp"
#include "scripted_behaviour.hpp"

template <>
CsScriptingSystem* Singleton<CsScriptingSystem>::currentInstance = nullptr;

CsScriptingSystem::CsScriptingSystem()
{
}

CsScriptingSystem::~CsScriptingSystem() = default;

MonoObject* GetStaticInstance(ScriptedBehaviour* managedInstance)
{
	return managedInstance->managedInstance->RawObject();
};

void CsScriptingSystem::Init()
{
	// TODO: Set generic pathes
	mono_set_dirs("lib", "externals\\etc");

	mono::ManagedScriptSystemSettings_t settings("RootDomain");
	m_scriptSystem = std::make_unique<mono::ManagedScriptSystem>(settings);

	MonoObject* (*lamb)(ScriptedBehaviour*) = [](ScriptedBehaviour* managedPtr)
	{
		return ((ScriptedBehaviour*)&managedPtr->GetHost())->managedInstance->RawObject();
	};

	mono_add_internal_call("CCEngine.ScriptedBehaviour::GetStaticInstance", GetStaticInstance);
}

std::string CsScriptingSystem::CopyTemporaryFile(const char* path)
{
	std::filesystem::path to(path);
	std::filesystem::path from = to;
	to.replace_extension("copy.dll");
	copy(from, to, std::filesystem::copy_options::update_existing);
	return to.generic_string().c_str();
}

mono::Ref<mono::ManagedScriptContext> CsScriptingSystem::CreateContext(char* domainName, const char* contextPath)
{
	std::string newPath = CopyTemporaryFile(contextPath);

	return m_scriptSystem->CreateContext(domainName, contextPath, newPath.c_str());
}

void CsScriptingSystem::ReloadContextes()
{
	auto contextes = m_scriptSystem->GetContextes();
	for (auto& [contextPath, contextRef] : contextes)
	{
		if (!contextRef->Unload())
			return;

		CopyTemporaryFile(contextRef->GetPath().c_str());

		contextRef->Init();
	}
}
