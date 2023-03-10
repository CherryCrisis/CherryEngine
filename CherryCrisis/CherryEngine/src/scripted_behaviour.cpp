#include <pch.hpp>

#include "scripted_behaviour.hpp"

#include "input_manager.hpp"
#include "resource_manager.hpp"
#include "csscripting_system.hpp"

#include "csassembly.hpp"
#include "csreflection_ex.hpp"
#include "entity.hpp"
#include "field.hpp"
#include "monowrapper.hpp"
#include "property.hpp"


ScriptedBehaviour::ScriptedBehaviour()
{
	m_scriptingAssembly = ResourceManager::GetInstance()->AddResource<CsAssembly>("CherryScripting.dll", false, "ScriptingDomain", true);
	m_interfaceAssembly = ResourceManager::GetInstance()->AddResource<CsAssembly>("CherryScriptInterface.dll", false, "InterfaceDomain", false);

	m_behaviourClass = m_interfaceAssembly->m_context->FindClass("CCEngine", "Behaviour");
	m_entityClass = m_interfaceAssembly->m_context->FindClass("CCEngine", "Entity");


	Behaviour::PopulateMetadatas();
	m_metadatas.SetProperty("scriptName", &scriptPath, "\0", false);

	if (m_scriptingAssembly)
		m_scriptingAssembly->m_OnDeleted.Bind(&ScriptedBehaviour::InvalidateAssembly, this);
}

ScriptedBehaviour::ScriptedBehaviour(CCUUID& id) : Behaviour(id)
{
	m_scriptingAssembly = ResourceManager::GetInstance()->AddResource<CsAssembly>("CherryScripting.dll", false, "ScriptingDomain", true);
	m_interfaceAssembly = ResourceManager::GetInstance()->AddResource<CsAssembly>("CherryScriptInterface.dll", false, "InterfaceDomain", false);

	m_behaviourClass = m_interfaceAssembly->m_context->FindClass("CCEngine", "Behaviour");
	m_entityClass = m_interfaceAssembly->m_context->FindClass("CCEngine", "Entity");

	Behaviour::PopulateMetadatas();
	m_metadatas.SetProperty("scriptName", &scriptPath, "\0", false);

	if (m_scriptingAssembly)
		m_scriptingAssembly->m_OnDeleted.Bind(&ScriptedBehaviour::InvalidateAssembly, this);
}

ScriptedBehaviour::~ScriptedBehaviour()
{
	m_properties.clear();
	m_metadatas.m_metadatas.clear();

	UnsetSignals();

	if (m_scriptingAssembly)
		m_scriptingAssembly->m_OnDeleted.Unbind(&ScriptedBehaviour::InvalidateAssembly, this);

	if (m_managedInstance)
		m_managedInstance->Dispose();
}

void ScriptedBehaviour::OnSetOwner(Entity* newOwner)
{
	if (m_linked && !m_subscribed && m_managedInstance)
	{
		m_subscribed = true;
		GetHost().SubscribeComponent(this, m_scriptName);
		SetSignals();
	}
}

void ScriptedBehaviour::SetSignals()
{
	// Bind all the signals if they have a managed equivalent
	if (m_managedUpdate)
		GetHost().m_OnTick.Bind(&ScriptedBehaviour::Update, this);

	if (m_managedStart)
		GetHost().m_OnStart.Bind(&ScriptedBehaviour::Start, this);

	if (m_managedAwake)
		GetHost().m_OnAwake.Bind(&ScriptedBehaviour::Awake, this);

	if (m_managedCollideIn)
		GetHost().m_OnCollisionEnter.Bind(&ScriptedBehaviour::OnCollisionEnter, this);

	if (m_managedCollideStay)
		GetHost().m_OnCollisionStay.Bind(&ScriptedBehaviour::OnCollisionStay, this);

	if (m_managedCollideOut)
		GetHost().m_OnCollisionExit.Bind(&ScriptedBehaviour::OnCollisionExit, this);

	if (m_managedTriggerIn)
		GetHost().m_OnTriggerEnter.Bind(&ScriptedBehaviour::OnTriggerEnter, this);

	if (m_managedTriggerOut)
		GetHost().m_OnTriggerExit.Bind(&ScriptedBehaviour::OnTriggerExit, this);
}

void ScriptedBehaviour::UnsetSignals()
{
	// Unbind all the signals if they had a managed equivalent

	if (m_managedUpdate)
		GetHost().m_OnTick.Unbind(&ScriptedBehaviour::Update, this);

	if (m_managedStart)
		GetHost().m_OnStart.Unbind(&ScriptedBehaviour::Start, this);

	if (m_managedAwake)
		GetHost().m_OnAwake.Unbind(&ScriptedBehaviour::Awake, this);

	if (m_managedCollideIn)
		GetHost().m_OnCollisionEnter.Unbind(&ScriptedBehaviour::OnCollisionEnter, this);

	if (m_managedCollideStay)
		GetHost().m_OnCollisionStay.Unbind(&ScriptedBehaviour::OnCollisionStay, this);

	if (m_managedCollideOut)
		GetHost().m_OnCollisionExit.Unbind(&ScriptedBehaviour::OnCollisionExit, this);

	if (m_managedTriggerIn)
		GetHost().m_OnTriggerEnter.Unbind(&ScriptedBehaviour::OnTriggerEnter, this);

	if (m_managedTriggerOut)
		GetHost().m_OnTriggerExit.Unbind(&ScriptedBehaviour::OnTriggerExit, this);
}

void ScriptedBehaviour::SetScriptClass(const std::string& scriptName)
{
	m_scriptName = scriptName;

	if (!m_scriptingAssembly || !m_scriptingAssembly->m_context)
		return;

	m_managedClass = m_scriptingAssembly->m_context->FindClass("CCScripting", scriptName.c_str());

	// Create all UnmanagedThunk for function signals if they have a managed equivalent

	if (m_managedUpdate = m_managedClass->FindMethod("Update"))
		csUpdate = m_managedUpdate->GetMemberUnmanagedThunk<void>();

	if (m_managedStart = m_managedClass->FindMethod("Start"))
		csStart = m_managedStart->GetMemberUnmanagedThunk<void>();

	if (m_managedAwake = m_managedClass->FindMethod("Awake"))
		csAwake = m_managedAwake->GetMemberUnmanagedThunk<void>();

	if (m_managedCollideIn = m_managedClass->FindMethod("OnCollisionEnter"))
		csCollideIn = m_managedCollideIn->GetMemberUnmanagedThunk<void, MonoObject*>();

	if (m_managedCollideStay = m_managedClass->FindMethod("OnCollisionStay"))
		csCollideStay = m_managedCollideStay->GetMemberUnmanagedThunk<void, MonoObject*>();

	if (m_managedCollideOut = m_managedClass->FindMethod("OnCollisionExit"))
		csCollideOut = m_managedCollideOut->GetMemberUnmanagedThunk<void, MonoObject*>();

	if (m_managedTriggerIn = m_managedClass->FindMethod("OnTriggerEnter"))
		csTriggerIn = m_managedTriggerIn->GetMemberUnmanagedThunk<void, MonoObject*>();

	if (m_managedTriggerOut = m_managedClass->FindMethod("OnTriggerExit"))
		csTriggerOut = m_managedTriggerOut->GetMemberUnmanagedThunk<void, MonoObject*>();

	m_managedInstance = m_managedClass->CreateUnmanagedInstance(this, false);

	if (Entity* host = GetHostPtr(); host && !m_subscribed)
	{
		m_subscribed = true;
		host->SubscribeComponent(this, m_scriptName);
		SetSignals();
	}
		
	if (!m_managedInstance)
		return;

	PopulateMetadatas();

	m_linked = true;
}

void ScriptedBehaviour::PopulateMetadatas()
{
	auto handleRefClass = m_scriptingAssembly->m_context->FindSystemClass("System.Runtime.InteropServices", "HandleRef");
	MonoProperty* getHandleProp = mono_class_get_property_from_name(handleRefClass, "Handle");
	MonoMethod* getHandleMethod = mono_property_get_get_method(getHandleProp);

	const auto& fields = m_managedClass->Fields();

	for (const auto& [fieldName, fieldRef] : fields)
	{
		mono::ManagedType* fieldType = fieldRef->Type();

		std::shared_ptr<CCProperty::IClearProperty> currentProperty;

		if (fieldType->Equals(mono::ManagedType::GetInt32()))
		{
			currentProperty = std::make_shared<ReflectedField<int>>(m_managedInstance, fieldRef.get());
		}
		else if (fieldType->Equals(mono::ManagedType::GetString()))
		{
			currentProperty = std::make_shared<ReflectedField<std::string>>(m_managedInstance, fieldRef.get());
		}
		else if (fieldType->Equals(mono::ManagedType::GetSingle()))
		{
			currentProperty = std::make_shared<ReflectedField<float>>(m_managedInstance, fieldRef.get());
		}
		else if (fieldType->Equals(mono::ManagedType::GetBoolean()))
		{
			currentProperty = std::make_shared<ReflectedField<bool>>(m_managedInstance, fieldRef.get());
		}

		if (currentProperty)
		{
			m_properties[fieldName] = currentProperty;
			m_metadatas.SetProperty(fieldName.c_str(), currentProperty.get());
			continue;
		}

		const std::string& name = fieldType->Name();

		const std::string& fullName = fieldType->Name();

		mono::ManagedClass* managedClass = m_interfaceAssembly->m_context->FindClass(fullName.c_str());
		if (!managedClass)
			managedClass = m_scriptingAssembly->m_context->FindClass(fullName.c_str());

		if (managedClass)
		if (mono::ManagedMethod* managedGetCPtr = managedClass->FindMethod("getCPtr"))
		{
			MonoObject* managedFieldInstance = nullptr, *excep = nullptr;
			m_managedInstance->GetField(fieldName.c_str(), &managedFieldInstance);

			MonoObject* ptrHandle = managedGetCPtr->InvokeStatic((void**)&managedFieldInstance, &excep);

			void* unboxedHandle = mono_object_unbox(ptrHandle);
			MonoObject* res = mono_runtime_invoke(getHandleMethod, unboxedHandle, nullptr, &excep);

			if (!res || excep)
				return;

			std::string identifier;

			if (fieldType->Equals("CCEngine.Vector3"))
			{
				CCMaths::Vector3* valPtr = *(CCMaths::Vector3**)mono_object_unbox(res);

				m_metadatas.SetFieldFromPtr(fieldName.c_str(), valPtr);
			}
			else if (fieldType->Equals("CCEngine.Entity"))
			{
				currentProperty = std::make_shared<ReflectedManagedObjectField<Entity*>>(m_managedInstance, fieldRef.get(), managedClass, getHandleMethod);
			}
			else if (fieldType->InheritOf(m_behaviourClass))
			{
				std::string compName = fieldType->Name().substr(fieldType->Name().find_last_of('.') + 1);

				identifier = compName;

				currentProperty = std::make_shared<ReflectedManagedObjectField<Behaviour*>>(m_managedInstance, fieldRef.get(), managedClass, getHandleMethod);
			}

			if (currentProperty)
			{
				m_properties[fieldName] = currentProperty;
				m_metadatas.SetProperty(fieldName.c_str(), currentProperty.get(), identifier.c_str());
			}
		}
	}

	const auto& properties = m_managedClass->Properties();

	for (const auto& [propName, propRef] : properties)
	{
		const mono::ManagedType* getType = propRef->GetType();
		const mono::ManagedType* setType = propRef->SetType();

		std::shared_ptr<CCProperty::IClearProperty> currentProperty;

		if (getType && getType->Equals(mono::ManagedType::GetInt32()) ||
			setType && setType->Equals(mono::ManagedType::GetInt32()))
		{
			currentProperty = std::make_shared<ReflectedProperty<int>>(m_managedInstance, propRef.get());
		}

		else if (getType && getType->Equals(mono::ManagedType::GetString()) ||
				setType && setType->Equals(mono::ManagedType::GetString()))
		{
			currentProperty = std::make_shared<ReflectedProperty<std::string>>(m_managedInstance, propRef.get());
		}

		else if (getType && getType->Equals(mono::ManagedType::GetSingle()) ||
				setType && setType->Equals(mono::ManagedType::GetSingle()))
		{
			currentProperty = std::make_shared<ReflectedProperty<float>>(m_managedInstance, propRef.get());
		}

		if (currentProperty)
		{
			m_properties[propName] = currentProperty;
			m_metadatas.SetProperty(propName.c_str(), currentProperty.get());
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

void ScriptedBehaviour::OnCollisionEnter(Entity* other)
{
	// Create a managed instance from the already existing entity
	// Save it in cache to avoir another instanciation in the Stay and Exit functions
	mono::ManagedObject* entityInstance = m_physicEntities[other] = m_entityClass->CreateUnmanagedInstance(other, false);

	MonoException* excep = nullptr;
	csCollideIn->Invoke(m_managedInstance->RawObject(), entityInstance->RawObject(), &excep);
}

void ScriptedBehaviour::OnCollisionStay(Entity* other)
{
	// Get the already existing managed Entity instance
	auto entityIt = m_physicEntities.find(other);
	if (entityIt == m_physicEntities.end())
		return;

	mono::ManagedObject* entityInstance = entityIt->second;

	MonoException* excep = nullptr;
	csCollideStay->Invoke(m_managedInstance->RawObject(), entityInstance->RawObject(), &excep);
}

void ScriptedBehaviour::OnCollisionExit(Entity* other)
{
	// Get the already existing managed Entity instance
	auto entityIt = m_physicEntities.find(other);
	if (entityIt == m_physicEntities.end())
		return;

	mono::ManagedObject* entityInstance = entityIt->second;

	MonoException* excep = nullptr;
	csCollideOut->Invoke(m_managedInstance->RawObject(), entityInstance->RawObject(), &excep);

	// Remove it from cache
	m_physicEntities.erase(entityIt);
}

void ScriptedBehaviour::OnTriggerEnter(Entity* other)
{
	// Create a managed instance from the already existing entity
	// Save it in cache to avoir another instanciation in the Exit function

	mono::ManagedObject* entityInstance = m_physicEntities[other] = m_entityClass->CreateUnmanagedInstance(other, false);

	MonoException* excep = nullptr;
	csTriggerIn->Invoke(m_managedInstance->RawObject(), entityInstance->RawObject(), &excep);
}

void ScriptedBehaviour::OnTriggerExit(Entity* other)
{
	// Get the already existing managed Entity instance
	auto entityIt = m_physicEntities.find(other);
	if (entityIt == m_physicEntities.end())
		return;

	mono::ManagedObject* entityInstance = entityIt->second;

	MonoException* excep = nullptr;
	csTriggerOut->Invoke(m_managedInstance->RawObject(), entityInstance->RawObject(), &excep);

	// Remove it from cache
	m_physicEntities.erase(entityIt);
}

_MonoObject* ScriptedBehaviour::GetRawInstance()
{
	return m_managedInstance->RawObject();
}
