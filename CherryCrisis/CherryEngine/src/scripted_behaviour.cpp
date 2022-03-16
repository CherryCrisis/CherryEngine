#include "pch.hpp"

#include <filesystem>
#include "scripted_behaviour.hpp"

#include "input_manager.hpp"
#include "csscripting_system.hpp"

ScriptedBehaviour::ScriptedBehaviour(Entity& owner)
	: Behaviour(owner)
{
	// TODO: Move this in the proper files
	std::filesystem::path to("x64/Debug/CherryScripting.dll");
	std::filesystem::path from = to;
	to.replace_extension("copy.dll");

	copy(from, to, std::filesystem::copy_options::update_existing);

	char domainName[16] = "ScriptingDomain";
	context = CsScriptingSystem::GetInstance()->m_scriptSystem->CreateContext(domainName, to.generic_string().c_str());

	CsScriptingSystem::GetInstance()->m_scriptSystem->DestroyContext(context.get());

	managedClass = context->FindClass("CCScripting", "BackpackBehaviour");
	auto managedVector = context->FindClass("CCEngine", "Vector3");
	auto handleRefClass = context->FindSystemClass("System.Runtime.InteropServices", "HandleRef");
	MonoProperty* getHandleProp = mono_class_get_property_from_name(handleRefClass, "Handle");
	MonoMethod* getHandleMethod = mono_property_get_get_method(getHandleProp);
	auto managedGetCPtr = managedVector->FindMethod("getCPtr");

	managedUpdate = managedClass->FindMethod("Update");
	managedStart = managedClass->FindMethod("Start");

	csUpdate = managedUpdate->GetMemberUnmanagedThunk<void>();
	csStart = managedStart->GetMemberUnmanagedThunk<void>();
	auto getCPtr = managedGetCPtr->GetStaticUnmanagedThunk<MonoObject*, MonoObject*>();

	behaviourInst = managedClass->CreateUnmanagedInstance(this, false);
	
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
	vecPtr->y = 90.f;
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

	if (!context->Unload())
		return;

	std::filesystem::path to("x64/Debug/CherryScripting.dll");
	std::filesystem::path from = to;
	to.replace_extension("copy.dll");
	copy(from, to, std::filesystem::copy_options::update_existing);

	if (!context->Init())
		return;

	MonoClass* intPtrType = context->FindSystemClass("System", "IntPtr");
	MonoClass* boolType = context->FindSystemClass("System", "Boolean");

	managedClass = context->FindClass("CCScripting", "BackpackBehaviour");

	managedUpdate = managedClass->FindMethod("Update");
	managedStart = managedClass->FindMethod("Start");

	csUpdate = (void(*)(MonoObject*, MonoException**))mono_method_get_unmanaged_thunk(managedUpdate->RawMethod());
	csStart = (void(*)(MonoObject*, MonoException**))mono_method_get_unmanaged_thunk(managedStart->RawMethod());

	bool value = false;
	int* ptr = (int*)this;
	void* args[] = { &ptr, &value };
	behaviourInst = managedClass->CreateInstance({ mono_class_get_type(intPtrType), mono_class_get_type(boolType) }, args);
}
