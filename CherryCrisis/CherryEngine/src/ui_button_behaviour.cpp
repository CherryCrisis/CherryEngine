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
	m_assembly = ResourceManager::GetInstance()->AddResource<CsAssembly>("CherryScripting.dll", true, "ScriptingDomain");
	m_metadatas.SetProperty("scriptName", &scriptPath);

	if (m_assembly)
	{
		m_assembly->m_OnReloaded.Bind(&ButtonBehaviour::Reload, this);
		m_assembly->m_OnDeleted.Bind(&ButtonBehaviour::InvalidateAssembly, this);
	}
}

ButtonBehaviour::ButtonBehaviour(CCUUID& id) : Behaviour(id)
{
	// TODO: Change path
	m_assembly = ResourceManager::GetInstance()->AddResource<CsAssembly>("CherryScripting.dll", true, "ScriptingDomain");
	m_metadatas.SetProperty("scriptName", &scriptPath);

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

	PopulateMetadatas();

	m_linked = true;
}

void ButtonBehaviour::PopulateMetadatas()
{
	Behaviour::PopulateMetadatas();

	m_metadatas.SetProperty("scriptName", &scriptPath);

	const auto& fields = managedClass->Fields();

	for (const auto& [fieldName, fieldRef] : fields)
	{
		mono::ManagedType* fieldType = fieldRef->Type();

		if (fieldType->Equals(mono::ManagedType::GetInt32()))
		{
			m_metadatas.SetProperty(fieldName.c_str(), new ReflectedField<int>(managedInstance, fieldRef.get()));
			continue;
		}

		if (fieldType->Equals(mono::ManagedType::GetString()))
		{
			m_metadatas.SetProperty(fieldName.c_str(), new ReflectedField<std::string>(managedInstance, fieldRef.get()));
			continue;
		}

		if (fieldType->Equals(mono::ManagedType::GetSingle()))
		{
			m_metadatas.SetProperty(fieldName.c_str(), new ReflectedField<float>(managedInstance, fieldRef.get()));
			continue;
		}
	}

	const auto& properties = managedClass->Properties();

	for (const auto& [propName, propRef] : properties)
	{
		const mono::ManagedType* getType = propRef->GetType();
		const mono::ManagedType* setType = propRef->SetType();

		if (getType && getType->Equals(mono::ManagedType::GetInt32()) ||
			setType && setType->Equals(mono::ManagedType::GetInt32()))
		{
			m_metadatas.SetProperty(propName.c_str(), new ReflectedProperty<int>(managedInstance, propRef.get()));
			continue;
		}

		if (getType && getType->Equals(mono::ManagedType::GetString()) ||
			setType && setType->Equals(mono::ManagedType::GetString()))
		{
			m_metadatas.SetProperty(propName.c_str(), new ReflectedProperty<std::string>(managedInstance, propRef.get()));
			continue;
		}

		if (getType && getType->Equals(mono::ManagedType::GetSingle()) ||
			setType && setType->Equals(mono::ManagedType::GetSingle()))
		{
			m_metadatas.SetProperty(propName.c_str(), new ReflectedProperty<float>(managedInstance, propRef.get()));
			continue;
		}
	}
}

void ButtonBehaviour::Reload(std::shared_ptr<CsAssembly> csAssembly)
{
	m_metadatas.m_fields.clear();
	m_metadatas.m_properties.clear();
	PopulateMetadatas();
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