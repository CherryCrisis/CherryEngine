#pragma once

#include "singleton.hpp"

#include "monowrapper.hpp"

class CsScriptingSystem : public Singleton<CsScriptingSystem>
{
	friend class Singleton<CsScriptingSystem>;

private:
	std::string CopyTemporaryFile(const char* path);

public:
	std::unique_ptr<mono::ManagedScriptSystem> m_scriptSystem;

	CsScriptingSystem();

	void Init(const mono::ManagedScriptSystemSettings_t& settings);

	mono::Ref<mono::ManagedScriptContext> CreateContext(char* domainName, const char* contextPath);
	void ReloadContextes();
};