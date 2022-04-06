#include "pch.hpp"

#include "csscripting_system.hpp"

#include "monowrapper.hpp"
#include "csassembly.hpp"
#include "scripted_behaviour.hpp"
#include "csassembly.hpp"

#include "utils.hpp"

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
	auto csassembly = ResourceManager::GetInstance()->AddResource<CsAssembly>("CherryScripting.dll", true, "ScriptingDomain");
	mono::ManagedAssembly* assembly = csassembly->m_context->FindAssembly("CherryScripting.copy.dll");
	mono::ManagedClass* behaviourClass = csassembly->m_context->FindClass("CCEngine", "Behaviour");
	csassembly->m_context->FindClass("CCScripting", "DebugTest");
	csassembly->m_context->FindClass("CCScripting", "BackpackBehaviour");
	csassembly->m_context->FindClass("CCScripting", "CameraController");

	auto behaviourTypeID = mono_type_get_type(behaviourClass->RawType());

	auto& classes = assembly->GetClasses();
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
