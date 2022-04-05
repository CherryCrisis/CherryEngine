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
	assembly = ResourceManager::GetInstance()->AddResource<CsAssembly>("../x64/Debug/CherryScripting.dll", true, "ScriptingDomain");
	m_metadatas.SetProperty("scriptName", &scriptPath);
}

ScriptedBehaviour::~ScriptedBehaviour()
{
	GetHost().m_OnStart.Unbind(&ScriptedBehaviour::Start, this);
	GetHost().m_OnTick.Unbind(&ScriptedBehaviour::Update, this);

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
}

void ScriptedBehaviour::SetScriptClass(const std::string& scriptName)
{
	m_scriptName = scriptName;

	if (!assembly || !assembly->context)
		return;

	managedClass = assembly->context->FindClass("CCScripting", scriptName.c_str());

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
	const auto& fields = managedClass->Fields();

	for (const auto& [fieldName, fieldRef] : fields)
	{
		if (fieldRef->Type()->Equals(mono::ManagedType::GetInt32()))
		{
			m_metadatas.SetProperty(fieldName.c_str(), new ReflectedField<int>(managedInstance, fieldRef.get()));
			continue;
		}

		if (fieldRef->Type()->Equals(mono::ManagedType::GetString()))
		{
			m_metadatas.SetProperty(fieldName.c_str(), new ReflectedField<std::string>(managedInstance, fieldRef.get()));
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