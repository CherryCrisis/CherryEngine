#include "pch.hpp"

#include "scripted_behaviour.hpp"

#include "input_manager.hpp"
#include "resource_manager.hpp"
#include "csscripting_system.hpp"

#include "csassembly.hpp"

#include "monowrapper.hpp"

ScriptedBehaviour::ScriptedBehaviour(Entity& owner)
	: Behaviour(owner)
{
	// TODO: Change path
	assembly = ResourceManager::GetInstance()->AddResource<CsAssembly>("../x64/Debug/CherryScripting.dll", true, "ScriptingDomain");
}

void ScriptedBehaviour::SetScriptClass(const char* scriptName)
{
	m_scriptName = scriptName;

	managedClass = assembly->context->FindClass("CCScripting", scriptName);

	managedUpdate = managedClass->FindMethod("Update");
	managedStart = managedClass->FindMethod("Start");

	csUpdate = managedUpdate->GetMemberUnmanagedThunk<void>();
	csStart = managedStart->GetMemberUnmanagedThunk<void>();

	behaviourInst = managedClass->CreateUnmanagedInstance(this, false);

	PopulateMetadatas();

	m_linked = true;
}

void ScriptedBehaviour::PopulateMetadatas()
{
	const auto& fields = managedClass->Fields();

	for (const auto& [fieldName, fieldRef] : fields)
	{
		if (fieldRef->Type()->Equals(mono::ManagedType::GetInt32()))
		{
			int val;
			behaviourInst->GetField(fieldRef.get(), &val);
			m_metadatas.m_fields.push_back({ fieldName, val });
			continue;
		}

		if (fieldRef->Type()->Equals(mono::ManagedType::GetString()))
		{
			MonoString* managedVal;
			behaviourInst->GetField(fieldRef.get(), &managedVal);
			char* monoChar = mono_string_to_utf8(managedVal);
			std::string stringVal = monoChar;
			mono_free(monoChar);

			m_metadatas.m_fields.push_back({ fieldName, stringVal });
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
	if (!m_linked)
		return;

	MonoException* excep = nullptr;
	csStart->Invoke(behaviourInst->RawObject(), &excep);
}

void ScriptedBehaviour::Update()
{
	if (!m_linked)
		return;

	MonoException* excep = nullptr;
	csUpdate->Invoke(behaviourInst->RawObject(), &excep);
}

void ScriptedBehaviour::Reload()
{
	managedClass = assembly->context->FindClass("CCScripting", "BackpackBehaviour");

	managedUpdate = managedClass->FindMethod("Update");
	managedStart = managedClass->FindMethod("Start");

	csUpdate = managedUpdate->GetMemberUnmanagedThunk<void>();
	csStart = managedStart->GetMemberUnmanagedThunk<void>();

	behaviourInst = managedClass->CreateUnmanagedInstance(this, false);
}
