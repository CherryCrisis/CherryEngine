#include "pch.hpp"

#include "scripted_behaviour.hpp"

ScriptedBehaviour::ScriptedBehaviour(Entity& owner)
	: Behaviour(owner)
{
	// TODO: Move this in the proper files
	mono_set_dirs(".\\lib", ".\\externals\\etc");

	mono::ManagedScriptSystemSettings_t settings("TestDomain");
	settings.configIsFile = false,
	settings.configData = nullptr,

	script = std::make_shared<mono::ManagedScriptSystem>(settings);

	context = script->CreateContext("../x64/Debug/CherryScripting.dll");

	managedClass = context->FindClass("CherryScripting", "MyComponent");

	MonoClass* intPtrType = context->FindSystemClass("System", "IntPtr");
	MonoClass* boolType = context->FindSystemClass("System", "Boolean");

	managedUpdate = managedClass->FindMethod("Update");
	managedStart = managedClass->FindMethod("Start");

	csUpdate = (void(*)(MonoObject*, MonoException**))mono_method_get_unmanaged_thunk(managedUpdate->RawMethod());
	csStart = (void(*)(MonoObject*, MonoException**))mono_method_get_unmanaged_thunk(managedStart->RawMethod());

	bool value = true;
	int* ptr = (int*)this;
	void* args[] = { &ptr, &value };
	behaviourInst = managedClass->CreateInstance({ mono_class_get_type(intPtrType), mono_class_get_type(boolType) }, args);
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
}