#include "pch.hpp"

#include "csscripting_system.hpp"

#include "monowrapper.hpp"
#include "csassembly.hpp"
#include "scripted_behaviour.hpp"
#include "csassembly.hpp"

#include "utils.hpp"
#include "command.hpp"

#include "resource_manager.hpp"

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

	m_scriptAssembly = ResourceManager::GetInstance()->AddResource<CsAssembly>("CherryScripting.dll", true, "ScriptingDomain", true);
	m_interfaceAssembly = ResourceManager::GetInstance()->AddResource<CsAssembly>("CherryScriptInterface.dll", true, "InterfaceDomain", false);
	
	mono::ManagedAssembly* scriptAssembly = m_scriptAssembly->m_context->FindAssembly("CherryScripting.copy.dll");
	mono::ManagedClass* behaviourClass = m_interfaceAssembly->m_context->FindClass("CCEngine", "Behaviour");

	m_scriptAssembly->m_context->FindClass("CCScripting", "DebugTest");
	m_scriptAssembly->m_context->FindClass("CCScripting", "BackpackBehaviour");
	m_scriptAssembly->m_context->FindClass("CCScripting", "CameraController");
	m_scriptAssembly->m_context->FindClass("CCScripting", "Script");

	auto behaviourTypeID = mono_type_get_type(behaviourClass->RawType());

	auto& classes = scriptAssembly->GetClasses();
	for (auto& [className, classRef] : classes)
	{
		if (className == "Behaviour")
			continue;
		
		MonoClass* classParentMDR = mono_class_get_parent(classRef->RawClass());
		MonoType* typeParentMDR = mono_class_get_type(classParentMDR);
		if (mono_type_get_type(typeParentMDR) == behaviourTypeID)
			classesName.push_back(className);
	}
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
	Resource<CsAssembly>::ReloadResource(m_scriptAssembly);
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

	mono::ManagedClass* theclass = m_scriptAssembly->m_context->FindClass("CCScripting", name);
	mono::ManagedClass* behaviourClass = m_interfaceAssembly->m_context->FindClass("CCEngine", "Behaviour");

	auto behaviourTypeID = mono_type_get_type(behaviourClass->RawType());


	MonoClass* classParentMDR = mono_class_get_parent(theclass->RawClass());
	MonoType* typeParentMDR = mono_class_get_type(classParentMDR);
	if (mono_type_get_type(typeParentMDR) == behaviourTypeID)
		classesName.push_back(name);
}