#pragma once

#include <memory>

#include "behaviour.hpp"

#include "monowrapper.hpp"

class ScriptedBehaviour : public Behaviour
{
	std::shared_ptr<mono::ManagedScriptSystem> script;
	mono::ManagedScriptContext* context;

	mono::ManagedClass* managedClass;
	mono::ManagedObject* behaviourInst;
	mono::ManagedMethod* managedUpdate;
	mono::ManagedMethod* managedStart;

	std::function <void(MonoObject* _this, MonoException** _excep)> csUpdate;
	std::function <void(MonoObject* _this, MonoException** _excep)> csStart;

public:
	ScriptedBehaviour(Entity& owner);

	void Start() override;
	void Update() override;
};