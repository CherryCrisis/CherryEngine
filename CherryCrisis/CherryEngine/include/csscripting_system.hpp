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

	std::unique_ptr<mono::ManagedScriptSystem> m_scriptSystem;

public:
	mono::ManagedScriptContext* CreateContext(char* domainName, const char* contextPath);

	CsScriptingSystem();
	virtual ~CsScriptingSystem();

	void Init();
	void ReloadContextes();
};