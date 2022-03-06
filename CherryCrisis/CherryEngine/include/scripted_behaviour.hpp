#pragma once

#include "behaviour.hpp"

#include <memory>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/class.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/loader.h>
#include <mono/metadata/mono-config.h>
#include <mono/metadata/mono-debug.h>
#include <mono/metadata/mono-gc.h>
#include <mono/metadata/profiler.h>
#include <mono/metadata/reflection.h>
#include "monowrapper.hpp"

class ScriptedBehaviour : public Behaviour
{
	MonoObject* behaviourInst;
	MonoMethod* updateMethod;

	std::shared_ptr<ManagedScriptSettings> settings;
	std::shared_ptr<ManagedScript> script;
	ManagedContext* context;
	MonoClass* behaviourClass;
	MonoMethodDesc* updateDesc;
	MonoMethodDesc* ctordesc;
	MonoMethod* ctor;

	void SetupInterface();
public:
	ScriptedBehaviour(Entity& owner);

	void Start() override;
	void Update() override;
};