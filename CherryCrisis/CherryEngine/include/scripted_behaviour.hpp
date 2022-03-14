#pragma once

#include <memory>

#include <cherry_macros.hpp>

#include "behaviour.hpp"
#include "monowrapper.hpp"

class CCENGINE_API ScriptedBehaviour : public Behaviour
{
	std::shared_ptr<mono::ManagedScriptSystem> script;
	mono::ManagedScriptContext* context;

	mono::Ref<mono::ManagedClass> managedClass;
	mono::Ref<mono::ManagedObject> behaviourInst;
	mono::Ref<mono::ManagedMethod> managedUpdate;
	mono::Ref<mono::ManagedMethod> managedStart;

	std::function <void(MonoObject* _this, MonoException** _excep)> csUpdate;
	std::function <void(MonoObject* _this, MonoException** _excep)> csStart;

	std::string name;
	int num;

public:
	ScriptedBehaviour(Entity& owner);

	void Start() override;
	void Update() override;

	void Reload();
};