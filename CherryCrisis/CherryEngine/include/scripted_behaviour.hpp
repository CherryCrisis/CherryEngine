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

	std::shared_ptr<class CsAssembly> m_assembly;
	mono::ManagedScriptSystem* script;
	mono::ManagedObject* managedInstance;
	mono::ManagedClass* managedClass;
	mono::ManagedMethod* managedUpdate;
	mono::ManagedMethod* managedStart;

	mono::ManagedThunk<void, struct _MonoObject*>* csUpdate;
	mono::ManagedThunk<void, struct _MonoObject*>* csStart;
	mono::ManagedThunk<void, struct _MonoObject*>* csCollideIn;
	mono::ManagedThunk<void, struct _MonoObject*>* csCollideStay;
	mono::ManagedThunk<void, struct _MonoObject*>* csCollideOut;
	mono::ManagedThunk<void, struct _MonoObject*>* csTriggerIn;
	mono::ManagedThunk<void, struct _MonoObject*>* csTriggerOut;

	void PopulateMetadatas() override;

	void InvalidateAssembly() { m_assembly = nullptr; }

public:
	ScriptedBehaviour();
	ScriptedBehaviour(CCUUID& id);
	~ScriptedBehaviour();

	void SetScriptClass(const std::string& scriptName);
	void BindToSignals() override;
	void Initialize();
	void Start();
	void Update();

	void OnCollisionEnter();
	void OnCollisionStay();
	void OnCollisionExit();
	void OnTriggerEnter();
	void OnTriggerExit();

	void Reload(std::shared_ptr<CsAssembly> csAssembly);

	std::string GetScriptPath() { return m_scriptName; }

	_MonoObject* GetRawInstance();

	CCProperty::ConstRefProperty<ScriptedBehaviour, std::string> scriptPath { this, &ScriptedBehaviour::SetScriptClass, &ScriptedBehaviour::GetScriptPath};
};