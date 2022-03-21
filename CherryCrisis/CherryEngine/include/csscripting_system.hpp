#pragma once

#include "cherry_macros.hpp"

#include "singleton.hpp"

namespace mono
{
	class ManagedScriptSystem;
	class ManagedScriptContext;
}

class CCENGINE_API CsScriptingSystem : public Singleton<CsScriptingSystem>
{
	friend class Singleton<CsScriptingSystem>;

private:
	std::string CopyTemporaryFile(const char* path);

public:
	std::unique_ptr<mono::ManagedScriptSystem> m_scriptSystem;

	CsScriptingSystem();
	virtual ~CsScriptingSystem();

	void Init();

	std::shared_ptr<mono::ManagedScriptContext> CreateContext(char* domainName, const char* contextPath);
	void ReloadContextes();
};