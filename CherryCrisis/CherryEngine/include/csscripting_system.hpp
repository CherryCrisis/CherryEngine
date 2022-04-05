#pragma once

#include <vector>

#include "cherry_macros.hpp"

#include "singleton.hpp"

namespace mono
{
	class ManagedScriptSystem;
	class ManagedScriptContext;
}

class CsAssembly;

class CCENGINE_API CsScriptingSystem : public Singleton<CsScriptingSystem>
{
	friend class Singleton<CsScriptingSystem>;

private:
	std::unique_ptr<mono::ManagedScriptSystem> m_scriptSystem;
	std::vector<std::shared_ptr<class CsAssembly>> m_assemblies;

public:
	mono::ManagedScriptContext* CreateContext(char* domainName, const char* contextPath);
	void InitializeAssembly(std::shared_ptr<class CsAssembly> assembly, const char* domainName);

	CsScriptingSystem();
	virtual ~CsScriptingSystem();

	void Init();
	void ReloadContextes();
};