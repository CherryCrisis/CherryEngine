#include "pch.hpp"

#include "scripted_behaviour.hpp"

#include "input_manager.hpp"
#include "resource_manager.hpp"
#include "csscripting_system.hpp"

#include "csassembly.hpp"

#include "monowrapper.hpp"

#include "csreflection_ex.hpp"

ScriptedBehaviour::ScriptedBehaviour()
{
	// TODO: Change path
	m_scriptingAssembly = ResourceManager::GetInstance()->AddResource<CsAssembly>("CherryScripting.dll", false, "ScriptingDomain", true);
	m_interfaceAssembly = ResourceManager::GetInstance()->AddResource<CsAssembly>("CherryScriptInterface.dll", false, "InterfaceDomain", false);

	m_metadatas.SetProperty("scriptName", &scriptPath);

	if (m_scriptingAssembly)
	{
		m_scriptingAssembly->m_OnReloaded.Bind(&ScriptedBehaviour::Reload, this);
		m_scriptingAssembly->m_OnDeleted.Bind(&ScriptedBehaviour::InvalidateAssembly, this);
	}
}

ScriptedBehaviour::ScriptedBehaviour(CCUUID& id) : Behaviour(id)
{
	// TODO: Change path
	m_scriptingAssembly = ResourceManager::GetInstance()->AddResource<CsAssembly>("CherryScripting.dll", false, "ScriptingDomain", true);
	m_interfaceAssembly = ResourceManager::GetInstance()->AddResource<CsAssembly>("CherryScriptInterface.dll", false, "InterfaceDomain", false);

	m_metadatas.SetProperty("scriptName", &scriptPath);

	if (m_scriptingAssembly)
	{
		m_scriptingAssembly->m_OnReloaded.Bind(&ScriptedBehaviour::Reload, this);
		m_scriptingAssembly->m_OnDeleted.Bind(&ScriptedBehaviour::InvalidateAssembly, this);
	}
}

ScriptedBehaviour::~ScriptedBehaviour()
{
	GetHost().m_OnStart.Unbind(&ScriptedBehaviour::Start, this);
	GetHost().m_OnTick.Unbind(&ScriptedBehaviour::Update, this);

	if (m_scriptingAssembly)
	{
		m_scriptingAssembly->m_OnReloaded.Unbind(&ScriptedBehaviour::Reload, this);
		m_scriptingAssembly->m_OnDeleted.Unbind(&ScriptedBehaviour::InvalidateAssembly, this);
	}

	if (m_managedInstance)
		m_managedInstance->Dispose();
}

void ScriptedBehaviour::BindToSignals() 
{
	if (!m_managedInstance)
		return;

	if (m_managedUpdate)
		GetHost().m_OnTick.Bind(&ScriptedBehaviour::Update, this);

	if (m_managedStart)
		GetHost().m_OnStart.Bind(&ScriptedBehaviour::Start, this);

	if (m_managedAwake)
		GetHost().m_OnAwake.Bind(&ScriptedBehaviour::Awake, this);

	GetHost().m_OnCollisionEnter.Bind(&ScriptedBehaviour::OnCollisionEnter, this);
	GetHost().m_OnCollisionStay.Bind(&ScriptedBehaviour::OnCollisionStay, this);
	GetHost().m_OnCollisionExit.Bind(&ScriptedBehaviour::OnCollisionExit, this);
	GetHost().m_OnTriggerEnter.Bind(&ScriptedBehaviour::OnTriggerEnter, this);
	GetHost().m_OnTriggerExit.Bind(&ScriptedBehaviour::OnTriggerExit, this);
}

void ScriptedBehaviour::SetScriptClass(const std::string& scriptName)
{
	m_scriptName = scriptName;

	if (!m_scriptingAssembly || !m_scriptingAssembly->m_context)
		return;

	m_managedClass = m_scriptingAssembly->m_context->FindClass("CCScripting", scriptName.c_str());

	if (m_managedUpdate = m_managedClass->FindMethod("Update"))
		csUpdate = m_managedUpdate->GetMemberUnmanagedThunk<void>();

	if (m_managedStart = m_managedClass->FindMethod("Start"))
		csStart = m_managedStart->GetMemberUnmanagedThunk<void>();

	if (m_managedAwake = m_managedClass->FindMethod("Awake"))
		csAwake= m_managedAwake->GetMemberUnmanagedThunk<void>();

	m_managedInstance = m_managedClass->CreateUnmanagedInstance(this, false);

	PopulateMetadatas();

	m_linked = true;
}

void ScriptedBehaviour::PopulateMetadatas()
{
	Behaviour::PopulateMetadatas();

	m_metadatas.SetProperty("scriptName", &scriptPath);

	auto handleRefClass = m_scriptingAssembly->m_context->FindSystemClass("System.Runtime.InteropServices", "HandleRef");
	MonoProperty* getHandleProp = mono_class_get_property_from_name(handleRefClass, "Handle");
	MonoMethod* getHandleMethod = mono_property_get_get_method(getHandleProp);

	const auto& fields = m_managedClass->Fields();

	for (const auto& [fieldName, fieldRef] : fields)
	{
		mono::ManagedType* fieldType = fieldRef->Type();

		if (fieldType->Equals(mono::ManagedType::GetInt32()))
		{
			// TODO: FREE ALLOCATIONS
			m_metadatas.SetProperty(fieldName.c_str(), new ReflectedField<int>(m_managedInstance, fieldRef.get()));
			continue;
		}

		if (fieldType->Equals(mono::ManagedType::GetString()))
		{
			m_metadatas.SetProperty(fieldName.c_str(), new ReflectedField<std::string>(m_managedInstance, fieldRef.get()));
			continue;
		}

		if (fieldType->Equals(mono::ManagedType::GetSingle()))
		{
			m_metadatas.SetProperty(fieldName.c_str(), new ReflectedField<float>(m_managedInstance, fieldRef.get()));
			continue;
		}

		auto name = fieldType->Name();

		const std::string& fullName = fieldType->Name();

		if (mono::ManagedClass* managedClass = m_interfaceAssembly->m_context->FindClass(fullName.c_str()))
		if (mono::ManagedMethod* managedGetCPtr = managedClass->FindMethod("getCPtr"))
		{
			MonoObject* managedFieldInstance = nullptr, *excep = nullptr;
			m_managedInstance->GetField(fieldName.c_str(), &managedFieldInstance);

			MonoObject* ptrHandle = managedGetCPtr->InvokeStatic((void**)&managedFieldInstance, &excep);

			void* unboxedHandle = mono_object_unbox(ptrHandle);
			MonoObject* res = mono_runtime_invoke(getHandleMethod, unboxedHandle, nullptr, &excep);

			if (!res || excep)
				return;

			if (fieldType->Equals("CCEngine.Vector3"))
			{
				CCMaths::Vector3* valPtr = *(CCMaths::Vector3**)mono_object_unbox(res);

				m_metadatas.SetFieldFromPtr(fieldName.c_str(), valPtr);
				continue;
			}

			if (fieldType->Equals("CCEngine.Transform"))
			{
				m_metadatas.SetProperty(fieldName.c_str(), new ReflectedField<Object*>(m_managedInstance, fieldRef.get(), managedClass, getHandleMethod));
				continue;
			}

			continue;
		}
	}

	const auto& properties = m_managedClass->Properties();

	for (const auto& [propName, propRef] : properties)
	{
		const mono::ManagedType* getType = propRef->GetType();
		const mono::ManagedType* setType = propRef->SetType();

		if (getType && getType->Equals(mono::ManagedType::GetInt32()) ||
			setType && setType->Equals(mono::ManagedType::GetInt32()))
		{
			m_metadatas.SetProperty(propName.c_str(), new ReflectedProperty<int>(m_managedInstance, propRef.get()));
			continue;
		}

		if (getType && getType->Equals(mono::ManagedType::GetString()) ||
			setType && setType->Equals(mono::ManagedType::GetString()))
		{
			m_metadatas.SetProperty(propName.c_str(), new ReflectedProperty<std::string>(m_managedInstance, propRef.get()));
			continue;
		}

		if (getType && getType->Equals(mono::ManagedType::GetSingle()) ||
			setType && setType->Equals(mono::ManagedType::GetSingle()))
		{
			m_metadatas.SetProperty(propName.c_str(), new ReflectedProperty<float>(m_managedInstance, propRef.get()));
			continue;
		}
	}
}

void ScriptedBehaviour::Awake()
{
	MonoException* excep = nullptr;
	csAwake->Invoke(m_managedInstance->RawObject(), &excep);

	GetHost().m_OnAwake.Unbind(&ScriptedBehaviour::Awake, this);
}

void ScriptedBehaviour::Start()
{
	MonoException* excep = nullptr;
	csStart->Invoke(m_managedInstance->RawObject(), &excep);

	GetHost().m_OnStart.Unbind(&ScriptedBehaviour::Start, this);
}

void ScriptedBehaviour::Update()
{
	MonoException* excep = nullptr;
	csUpdate->Invoke(m_managedInstance->RawObject(), &excep);
}

void ScriptedBehaviour::OnCollisionEnter()
{
	//MonoException* excep = nullptr;
	//csCollideIn->Invoke(managedInstance->RawObject(), &excep);
}

void ScriptedBehaviour::OnCollisionStay()
{
	//MonoException* excep = nullptr;
	//csCollideStay->Invoke(managedInstance->RawObject(), &excep);
}

void ScriptedBehaviour::OnCollisionExit()
{
	//MonoException* excep = nullptr;
	//csCollideOut->Invoke(managedInstance->RawObject(), &excep);
}

void ScriptedBehaviour::OnTriggerEnter()
{
	//MonoException* excep = nullptr;
	//csTriggerIn->Invoke(managedInstance->RawObject(), &excep);
}

void ScriptedBehaviour::OnTriggerExit()
{
	//MonoException* excep = nullptr;
	//csTriggerOut->Invoke(managedInstance->RawObject(), &excep);
}

void ScriptedBehaviour::Reload(std::shared_ptr<CsAssembly> csAssembly)
{
	m_metadatas.m_fields.clear();
	m_metadatas.m_properties.clear();
	PopulateMetadatas();
}

_MonoObject* ScriptedBehaviour::GetRawInstance()
{
	return m_managedInstance->RawObject();
}
