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

	mono::ManagedClass* entClass = context->FindClass("CCEngine", "Entity");

	bool value = false;
	void* params[] = { &GetHost(), &value };
	mono::ManagedObject* entObj = entClass->CreateInstance({ mono_class_get_type(intPtrType), mono_class_get_type(boolType) }, params);

	update = managedClass->FindMethod("Update");

	void* args[] = { entObj->RawObject() };
	behaviourInst = managedClass->CreateInstance({ entClass->RawType() }, args);
}

void ScriptedBehaviour::SetupInterface()
{

}

void ScriptedBehaviour::Start()
{
	SetupInterface();
}

void ScriptedBehaviour::Update()
{
	update->Invoke(behaviourInst, nullptr);
}