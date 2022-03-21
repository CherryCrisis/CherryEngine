#include "pch.hpp"

#include "csscripting_system.hpp"

#include <filesystem>

template <>
CsScriptingSystem* Singleton<CsScriptingSystem>::currentInstance = nullptr;

CsScriptingSystem::CsScriptingSystem()
{
	// TODO: Set generic pathes
	mono_set_dirs("lib", "externals\\etc");
}

void CsScriptingSystem::Init(const mono::ManagedScriptSystemSettings_t& settings)
{
	m_scriptSystem = std::make_unique<mono::ManagedScriptSystem>(settings);
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
