#include "pch.hpp"

#include <filesystem>
#include "scripted_behaviour.hpp"

#include "input_manager.hpp"

ScriptedBehaviour::ScriptedBehaviour(Entity& owner)
	: Behaviour(owner)
{
	// TODO: Move this in the proper files
	mono_set_dirs("CherryEditor\\lib", "CherryEditor\\externals\\etc");

	mono::ManagedScriptSystemSettings_t settings("TestDomain");

	script = std::make_shared<mono::ManagedScriptSystem>(settings);

	std::filesystem::path to("x64/Debug/CherryScripting.dll");
	std::filesystem::path from = to;
	to.replace_extension("copy.dll");

	copy(from, to, std::filesystem::copy_options::update_existing);

	char domainName[16] = "ScriptingDomain";
	context = script->CreateContext(domainName, to.generic_string().c_str());

	managedClass = context->FindClass("CCScripting", "BackpackBehaviour");
	
	MonoClass* intPtrType = mono_get_intptr_class();
	MonoClass* boolType = mono_get_boolean_class();

	managedUpdate = managedClass->FindMethod("Update");
	managedStart = managedClass->FindMethod("Start");

	csUpdate = (void(*)(MonoObject*, MonoException**))mono_method_get_unmanaged_thunk(managedUpdate->RawMethod());
	csStart = (void(*)(MonoObject*, MonoException**))mono_method_get_unmanaged_thunk(managedStart->RawMethod());

	bool value = false;
	int* ptr = (int*)this;
	void* args[] = { &ptr, &value };
	behaviourInst = managedClass->CreateInstance({ mono_class_get_type(intPtrType), mono_class_get_type(boolType) }, args);

	const char* nameptr = &name[0];
	behaviourInst->GetField("name", &(nameptr));

	behaviourInst->GetField("num", &num);

	MonoClass* int16Class = mono_get_int16_class();
	MonoClass* int16Classbis = mono_get_int16_class();
	MonoType* int16Type = mono_class_get_type(int16Class);
	MonoType* int16Typebis = mono_class_get_type(int16Classbis);

	MonoClass* int32Class = mono_get_int32_class();
	MonoType* int32Type = mono_class_get_type(int32Class);

	MonoClass* int64Class = mono_get_int64_class();
	MonoType* int64Type = mono_class_get_type(int64Class);

	MonoClass* stringClass = mono_get_string_class();
	MonoType* stringType = mono_class_get_type(stringClass);

	std::string ns = mono_class_get_namespace(mono_get_int16_class());

	auto numFieldRef = managedClass->FindField("num");
	mono::ManagedType* numType = numFieldRef->Type();

	auto nameFieldRef = managedClass->FindField("name");
	mono::ManagedType* nameType = nameFieldRef->Type();

	bool isEqual = nameType->Equals(mono::ManagedType::GetString());
	bool isEqual2 = nameType->Equals(mono::ManagedType::GetEnum());
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
