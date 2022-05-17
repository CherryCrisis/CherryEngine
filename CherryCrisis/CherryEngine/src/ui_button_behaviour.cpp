#include "pch.hpp"

#include "ui_button_behaviour.hpp"

#include "input_manager.hpp"
#include "resource_manager.hpp"
#include "csscripting_system.hpp"

#include "csassembly.hpp"

#include "monowrapper.hpp"

#include "csreflection_ex.hpp"

ButtonBehaviour::ButtonBehaviour()
{
	// TODO: Change path
	m_assembly = ResourceManager::GetInstance()->AddResource<CsAssembly>("CherryScripting.dll", true, "ScriptingDomain", true);

	if (m_assembly)
	{
		m_assembly->m_OnReloaded.Bind(&ButtonBehaviour::Reload, this);
		m_assembly->m_OnDeleted.Bind(&ButtonBehaviour::InvalidateAssembly, this);
	}
}

ButtonBehaviour::ButtonBehaviour(CCUUID& id)
{
	// TODO: Change path
	m_assembly = ResourceManager::GetInstance()->AddResource<CsAssembly>("CherryScripting.dll", true, "ScriptingDomain", true);

	if (m_assembly)
	{
		m_assembly->m_OnReloaded.Bind(&ButtonBehaviour::Reload, this);
		m_assembly->m_OnDeleted.Bind(&ButtonBehaviour::InvalidateAssembly, this);
	}
}

ButtonBehaviour::~ButtonBehaviour()
{

	if (m_assembly)
	{
		m_assembly->m_OnReloaded.Unbind(&ButtonBehaviour::Reload, this);
		m_assembly->m_OnDeleted.Unbind(&ButtonBehaviour::InvalidateAssembly, this);
	}

	if (managedInstance)
		managedInstance->Dispose();
}

void ButtonBehaviour::SetScriptClass(const std::string& scriptName)
{
	m_scriptName = scriptName;

	if (!m_assembly || !m_assembly->m_context)
		return;

	managedClass = m_assembly->m_context->FindClass("CCScripting", scriptName.c_str());

	if (managedOnClick = managedClass->FindMethod("OnClick"))
		csOnClick = managedOnClick->GetMemberUnmanagedThunk<void>();

	managedInstance = managedClass->CreateUnmanagedInstance(this, false);


	m_linked = true;
}

void ButtonBehaviour::Reload(std::shared_ptr<CsAssembly> csAssembly)
{

}

_MonoObject* ButtonBehaviour::GetRawInstance()
{
	return managedInstance->RawObject();
}

void ButtonBehaviour::OnClick() 
{
	MonoException* excep = nullptr;
	csOnClick->Invoke(managedInstance->RawObject(), &excep);
}