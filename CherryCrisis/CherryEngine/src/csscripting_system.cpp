#include <pch.hpp>

#include "csscripting_system.hpp"

#include "resource_manager.hpp"

#include "command.hpp"
#include "csassembly.hpp"
#include "monowrapper.hpp"
#include "scripted_behaviour.hpp"
#include "scene_manager.hpp"
#include "utils.hpp"


template <>
CsScriptingSystem* Singleton<CsScriptingSystem>::currentInstance = nullptr;


CsScriptingSystem::CsScriptingSystem() = default;
CsScriptingSystem::~CsScriptingSystem() = default;

void CsScriptingSystem::Init()
{
	mono_set_dirs("lib", "externals\\etc");

	mono::ManagedScriptSystemSettings_t settings("RootDomain");
	m_scriptSystem = std::make_unique<mono::ManagedScriptSystem>(settings);

	MonoObject* (*GetStaticInstance)(ScriptedBehaviour*) = [](ScriptedBehaviour* managedPtr)
	{
		return managedPtr->GetRawInstance();
	};

	mono_add_internal_call("CCEngine.ScriptedBehaviour::GetStaticInstance", GetStaticInstance);

	m_scriptAssembly = ResourceManager::GetInstance()->AddResource<CsAssembly>("CherryScripting.dll", true, "ScriptingDomain", true);
	m_scriptAssembly->m_OnPreReloaded.Bind(&CsScriptingSystem::OnPreReload, this);
	m_scriptAssembly->m_OnReloaded.Bind(&CsScriptingSystem::OnReload, this);

	m_interfaceAssembly = ResourceManager::GetInstance()->AddResource<CsAssembly>("CherryScriptInterface.dll", true, "InterfaceDomain", false);
	m_behaviourClass = m_interfaceAssembly->m_context->FindClass("CCEngine", "Behaviour");
}

mono::ManagedScriptContext* CsScriptingSystem::CreateContext(char* domainName, const char* contextPath, bool makeCopy)
{
	std::string currPath = contextPath;

	if (makeCopy)
	{
		auto newPath = CopyTemporaryFile(contextPath);
		currPath = newPath.generic_string();
	}

	return m_scriptSystem->CreateContext(domainName, contextPath, currPath.c_str());
}

void CsScriptingSystem::InitializeAssembly(std::shared_ptr<CsAssembly> assembly, const char* domainName, bool makeCopy)
{
	char* name = (char*)&domainName[0];
	assembly->m_context = CreateContext(name, assembly->GetFilepath().c_str(), makeCopy);

	m_assemblies.push_back(assembly);
}

void CsScriptingSystem::ReloadContextes()
{
	Resource<CsAssembly>::ReloadResource(m_scriptAssembly.get());
}

void CsScriptingSystem::OnPreReload(CsAssembly* assembly)
{
	SceneManager::FlipScene();
	SceneManager::EmptyScene();
}

void CsScriptingSystem::OnReload(CsAssembly* assembly)
{
	SceneManager::ResetScene();
}

void CsScriptingSystem::InitializeHotReload(const char* compilerPath, const char* solutionPath) 
{
	classesName.clear();
	"\"C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\Common7\\IDE\\devenv\"";
	std::string fullArg = solutionPath + std::string(" /Build");
	callEx("open", "C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\Common7\\IDE\\devenv", fullArg.c_str());
	ReloadContextes();
}

void CsScriptingSystem::SubmitScript(const char* name) 
{
	if (std::find(classesName.begin(), classesName.end(), std::string(name)) != classesName.end())
		return;

	mono::ManagedClass* managedCustomClass = m_scriptAssembly->m_context->FindClass("CCScripting", name);

	if (!managedCustomClass || !m_behaviourClass)
		return;

	auto behaviourTypeID = mono_type_get_type(m_behaviourClass->RawType());

	MonoClass* parentClass = mono_class_get_parent(managedCustomClass->RawClass());
	MonoType* parentType = mono_class_get_type(parentClass);

	if (mono_type_get_type(parentType) == behaviourTypeID)
		classesName.push_back(name);
}