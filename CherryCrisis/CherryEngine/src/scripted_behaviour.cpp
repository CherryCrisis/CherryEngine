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
	m_assembly = ResourceManager::GetInstance()->AddResource<CsAssembly>("CherryScripting.dll", true, "ScriptingDomain");
	m_metadatas.SetProperty("scriptName", &scriptPath);

	if (m_assembly)
	{
		m_assembly->m_OnReloaded.Bind(&ScriptedBehaviour::Reload, this);
		m_assembly->m_OnDeleted.Bind(&ScriptedBehaviour::InvalidateAssembly, this);
	}
}

ScriptedBehaviour::ScriptedBehaviour(CCUUID& id) : Behaviour(id)
{
	// TODO: Change path
	m_assembly = ResourceManager::GetInstance()->AddResource<CsAssembly>("CherryScripting.dll", true, "ScriptingDomain");
	m_metadatas.SetProperty("scriptName", &scriptPath);

	if (m_assembly)
	{
		m_assembly->m_OnReloaded.Bind(&ScriptedBehaviour::Reload, this);
		m_assembly->m_OnDeleted.Bind(&ScriptedBehaviour::InvalidateAssembly, this);
	}
}

ScriptedBehaviour::~ScriptedBehaviour()
{
	GetHost().m_OnStart.Unbind(&ScriptedBehaviour::Start, this);
	GetHost().m_OnTick.Unbind(&ScriptedBehaviour::Update, this);

	if (m_assembly) 
	{
		m_assembly->m_OnReloaded.Unbind(&ScriptedBehaviour::Reload, this);
		m_assembly->m_OnDeleted.Unbind(&ScriptedBehaviour::InvalidateAssembly, this);
	}

	if (managedInstance)
		managedInstance->Dispose();
}

void ScriptedBehaviour::BindToSignals() 
{
	if (!managedInstance)
		return;

	if (managedUpdate)
		GetHost().m_OnTick.Bind(&ScriptedBehaviour::Update, this);

	if (managedStart)
		GetHost().m_OnStart.Bind(&ScriptedBehaviour::Start, this);

	GetHost().m_OnCollisionEnter.Bind(&ScriptedBehaviour::OnCollisionEnter, this);
	GetHost().m_OnCollisionStay.Bind(&ScriptedBehaviour::OnCollisionStay, this);
	GetHost().m_OnCollisionExit.Bind(&ScriptedBehaviour::OnCollisionExit, this);
	GetHost().m_OnTriggerEnter.Bind(&ScriptedBehaviour::OnTriggerEnter, this);
	GetHost().m_OnTriggerExit.Bind(&ScriptedBehaviour::OnTriggerExit, this);
}

void ScriptedBehaviour::SetScriptClass(const std::string& scriptName)
{
	m_scriptName = scriptName;

	if (!m_assembly || !m_assembly->m_context)
		return;

	managedClass = m_assembly->m_context->FindClass("CCScripting", scriptName.c_str());

	if (managedUpdate = managedClass->FindMethod("Update"))
		csUpdate = managedUpdate->GetMemberUnmanagedThunk<void>();

	if (managedStart = managedClass->FindMethod("Start"))
		csStart = managedStart->GetMemberUnmanagedThunk<void>();

	managedInstance = managedClass->CreateUnmanagedInstance(this, false);

	PopulateMetadatas();

	m_linked = true;
}

void ScriptedBehaviour::PopulateMetadatas()
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

	/*auto handleRefClass = assembly->context->FindSystemClass("System.Runtime.InteropServices", "HandleRef");
	MonoProperty* getHandleProp = mono_class_get_property_from_name(handleRefClass, "Handle");
	MonoMethod* getHandleMethod = mono_property_get_get_method(getHandleProp);

	auto managedVector = assembly->context->FindClass("CCEngine", "Vector3");
	auto managedGetCPtr = managedVector->FindMethod("getCPtr");
	auto getCPtr = managedGetCPtr->GetStaticUnmanagedThunk<MonoObject*, MonoObject*>();
	MonoObject* managedVec = nullptr;
	behaviourInst->GetField("pos", &managedVec);

	MonoException* excep = nullptr;
	MonoObject* ptrHandle = getCPtr->Invoke(managedVec, &excep);

	void* unboxedHandle = mono_object_unbox(ptrHandle);

	MonoObject* exception = nullptr;
	MonoObject* res = mono_runtime_invoke(getHandleMethod, unboxedHandle, nullptr, &exception);

	if (!res || exception)
		return;

	CCMaths::Vector3* vecPtr = *(CCMaths::Vector3**)mono_object_unbox(res);*/
}

void ScriptedBehaviour::Start()
{
	MonoException* excep = nullptr;
	csStart->Invoke(managedInstance->RawObject(), &excep);

	GetHost().m_OnStart.Unbind(&ScriptedBehaviour::Start, this);
}

void ScriptedBehaviour::Update()
{
	MonoException* excep = nullptr;
	csUpdate->Invoke(managedInstance->RawObject(), &excep);
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
	return managedInstance->RawObject();
}
