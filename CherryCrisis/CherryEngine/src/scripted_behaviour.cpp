#include "pch.hpp"

#include "scripted_behaviour.hpp"

#include "input_manager.hpp"
#include "resource_manager.hpp"
#include "csscripting_system.hpp"

#include "csassembly.hpp"

#include "monowrapper.hpp"

#include "csreflection_ex.hpp"

ScriptedBehaviour::ScriptedBehaviour(Entity& owner)
	: Behaviour(owner)
{
	// TODO: Change path
	assembly = ResourceManager::GetInstance()->AddResource<CsAssembly>("../x64/Debug/CherryScripting.dll", true, "ScriptingDomain");

}

ScriptedBehaviour::~ScriptedBehaviour()
{
	GetHost().m_OnStart.Unbind(&ScriptedBehaviour::Start, this);
	GetHost().m_OnTick.Unbind(&ScriptedBehaviour::Update, this);
}


void ScriptedBehaviour::SetScriptClass(const char* scriptName)
{
	m_scriptName = scriptName;

	managedClass = assembly->context->FindClass("CCScripting", scriptName);

	if (managedUpdate = managedClass->FindMethod("Update"))
	{
		csUpdate = managedUpdate->GetMemberUnmanagedThunk<void>();
		GetHost().m_OnTick.Bind(&ScriptedBehaviour::Update, this);
	}

	if (managedStart = managedClass->FindMethod("Start"))
	{
		csStart = managedStart->GetMemberUnmanagedThunk<void>();
		GetHost().m_OnStart.Bind(&ScriptedBehaviour::Start, this);
	}

	managedInstance = managedClass->CreateUnmanagedInstance(this, false);

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
			m_metadatas.SetProperty(fieldName.c_str(), new ReflectedField<int>(managedInstance.get(), fieldRef.get()));
			continue;
		}

		if (fieldRef->Type()->Equals(mono::ManagedType::GetString()))
		{
			m_metadatas.SetProperty(fieldName.c_str(), new ReflectedField<std::string>(managedInstance.get(), fieldRef.get()));
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

void ScriptedBehaviour::Reload()
{
	managedClass = assembly->context->FindClass("CCScripting", "BackpackBehaviour");

	managedUpdate = managedClass->FindMethod("Update");
	managedStart = managedClass->FindMethod("Start");

	csUpdate = managedUpdate->GetMemberUnmanagedThunk<void>();
	csStart = managedStart->GetMemberUnmanagedThunk<void>();

	managedInstance = managedClass->CreateUnmanagedInstance(this, false);
}

_MonoObject* ScriptedBehaviour::GetRawInstance()
{
	return managedInstance->RawObject();
}