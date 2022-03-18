#include "pch.hpp"

#include "scripted_behaviour.hpp"

#include "input_manager.hpp"
#include "csscripting_system.hpp"

#include "monowrapper.hpp"

ScriptedBehaviour::ScriptedBehaviour(Entity& owner)
	: Behaviour(owner)
{
	char domainName[16] = "ScriptingDomain";
	// TODO: Change path
	context = CsScriptingSystem::GetInstance()->CreateContext(domainName, "x64/Debug/CherryScripting.dll");
	managedClass = context->FindClass("CCScripting", "BackpackBehaviour");

	managedUpdate = managedClass->FindMethod("Update");
	managedStart = managedClass->FindMethod("Start");

	csUpdate = managedUpdate->GetMemberUnmanagedThunk<void>();
	csStart = managedStart->GetMemberUnmanagedThunk<void>();

	behaviourInst = managedClass->CreateUnmanagedInstance(this, false);

	PopulateMetadatas();
}

void ScriptedBehaviour::PopulateMetadatas()
{
	auto handleRefClass = context->FindSystemClass("System.Runtime.InteropServices", "HandleRef");
	MonoProperty* getHandleProp = mono_class_get_property_from_name(handleRefClass, "Handle");
	MonoMethod* getHandleMethod = mono_property_get_get_method(getHandleProp);

	auto managedVector = context->FindClass("CCEngine", "Vector3");
	auto managedGetCPtr = managedVector->FindMethod("getCPtr");
	auto getCPtr = managedGetCPtr->GetStaticUnmanagedThunk<MonoObject*, MonoObject*>();

	int val;
	behaviourInst->GetField("num", &val);

	MonoString* name;
	behaviourInst->GetField("name", &name);

	MonoObject* managedVec = nullptr;
	behaviourInst->GetField("pos", &managedVec);

	MonoException* excep = nullptr;
	MonoObject* ptrHandle = getCPtr(managedVec, &excep);

	void* unboxedHandle = mono_object_unbox(ptrHandle);

	MonoObject* exception = nullptr;
	MonoObject* res = mono_runtime_invoke(getHandleMethod, unboxedHandle, nullptr, &exception);

	if (!res || exception)
		return;

	CCMaths::Vector3* vecPtr = *(CCMaths::Vector3**)mono_object_unbox(res);

	m_metadatas.m_fields.push_back({ "pos", DescriptorType::VECTOR3, vecPtr, true });
	m_metadatas.m_fields.push_back({ "num", DescriptorType::INT, val, false });
}

void ScriptedBehaviour::Start()
{
	MonoException* excep = nullptr;
	csStart(behaviourInst->RawObject(), &excep);
}

void ScriptedBehaviour::Update()
{
	MonoException* excep = nullptr;
	csUpdate(behaviourInst->RawObject(), &excep);

	if (InputManager::GetInstance()->GetKeyDown(Keycode::R))
		Reload();
}

void ScriptedBehaviour::Reload()
{
	auto csDispose = (void(*)(MonoObject*, bool, MonoException**))mono_method_get_unmanaged_thunk(managedClass->FindMethod("Dispose")->RawMethod());

	MonoException* excep = nullptr;
	csDispose(behaviourInst->RawObject(), true, &excep);

	CsScriptingSystem::GetInstance()->ReloadContextes();

	MonoClass* intPtrType = context->FindSystemClass("System", "IntPtr");
	MonoClass* boolType = context->FindSystemClass("System", "Boolean");

	managedClass = context->FindClass("CCScripting", "BackpackBehaviour");

	managedUpdate = managedClass->FindMethod("Update");
	managedStart = managedClass->FindMethod("Start");

	csUpdate = managedUpdate->GetMemberUnmanagedThunk<void>();
	csStart = managedStart->GetMemberUnmanagedThunk<void>();

	behaviourInst = managedClass->CreateUnmanagedInstance(this, false);
}
