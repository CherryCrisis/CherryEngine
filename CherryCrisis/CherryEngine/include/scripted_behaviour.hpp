#pragma once

#include <memory>
#include <functional>

#include <cherry_macros.hpp>

#include "behaviour.hpp"

namespace mono
{
	class ManagedScriptSystem;
	class ManagedClass;
	class ManagedObject;
	class ManagedMethod;
}

struct _MonoObject;
struct _MonoException;

class CCENGINE_API ScriptedBehaviour : public Behaviour
{
	std::shared_ptr<class CsAssembly> assembly;
	std::shared_ptr<mono::ManagedScriptSystem> script;

	std::shared_ptr<mono::ManagedClass> managedClass;
	std::shared_ptr<mono::ManagedObject> behaviourInst;
	std::shared_ptr<mono::ManagedMethod> managedUpdate;
	std::shared_ptr<mono::ManagedMethod> managedStart;

	std::function<void(_MonoObject* _this, _MonoException** _excep)> csUpdate;
	std::function<void(_MonoObject* _this, _MonoException** _excep)> csStart;

	void PopulateMetadatas() override;

public:
	ScriptedBehaviour(Entity& owner);

	void SetScriptClass(const char* scriptName);

	void Start() override;
	void Update() override;

	void Reload();
};