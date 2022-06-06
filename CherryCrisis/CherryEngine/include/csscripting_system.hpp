#pragma once

#include <vector>

#include "cherry_macros.hpp"

#include "singleton.hpp"

#include "event.hpp"

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
	std::vector<std::shared_ptr<CsAssembly>> m_assemblies;

	std::shared_ptr<CsAssembly> m_interfaceAssembly;
	std::shared_ptr<CsAssembly> m_scriptAssembly;

public:
	Event<> onReload;
	std::vector<std::string> classesName;

	mono::ManagedScriptContext* CreateContext(char* domainName, const char* contextPath, bool makeCopy);
	void InitializeAssembly(std::shared_ptr<CsAssembly> assembly, const char* domainName, bool makeCopy);

	CsScriptingSystem();
	virtual ~CsScriptingSystem();

	void Init();
	void ReloadContextes();

	void OnPreReload(std::shared_ptr<CsAssembly> assembly);
	void OnReload(std::shared_ptr<CsAssembly> assembly);
	
	void InitializeHotReload(const char* compilerPath, const char* solutionPath);

	void SubmitScript(const char* name);
};