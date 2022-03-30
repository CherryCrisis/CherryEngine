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
	template <typename RetT, typename ...Args>
	class ManagedThunk;
}

struct _MonoException;

class CCENGINE_API ScriptedBehaviour : public Behaviour
{
	std::string m_scriptName;

	bool m_linked = false;

	std::shared_ptr<class CsAssembly> assembly;
	std::shared_ptr<mono::ManagedScriptSystem> script;
	std::shared_ptr<mono::ManagedObject> managedInstance;
	std::shared_ptr<mono::ManagedClass> managedClass;
	std::shared_ptr<mono::ManagedMethod> managedUpdate;
	std::shared_ptr<mono::ManagedMethod> managedStart;

	mono::ManagedThunk<void, struct _MonoObject*>* csUpdate;
	mono::ManagedThunk<void, struct _MonoObject*>* csStart;

	void PopulateMetadatas() override;

public:

	ScriptedBehaviour(Entity& owner);
	~ScriptedBehaviour();

	void SetScriptClass(const char* scriptName);

	void Start() override;
	void Update() override;

	void Reload();

	std::string GetScriptName() { return m_scriptName; }

	_MonoObject* GetRawInstance();
};