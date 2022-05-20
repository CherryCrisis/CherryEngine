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

	std::shared_ptr<class CsAssembly>	m_scriptingAssembly;
	std::shared_ptr<class CsAssembly>	m_interfaceAssembly;
	mono::ManagedObject*				m_managedInstance = nullptr;
	mono::ManagedClass*					m_managedClass = nullptr;
	mono::ManagedMethod*				m_managedUpdate = nullptr;
	mono::ManagedMethod*				m_managedAwake = nullptr;
	mono::ManagedMethod*				m_managedStart = nullptr;

	mono::ManagedThunk<void, struct _MonoObject*>* csUpdate;
	mono::ManagedThunk<void, struct _MonoObject*>* csAwake;
	mono::ManagedThunk<void, struct _MonoObject*>* csStart;
	mono::ManagedThunk<void, struct _MonoObject*>* csCollideIn;
	mono::ManagedThunk<void, struct _MonoObject*>* csCollideStay;
	mono::ManagedThunk<void, struct _MonoObject*>* csCollideOut;
	mono::ManagedThunk<void, struct _MonoObject*>* csTriggerIn;
	mono::ManagedThunk<void, struct _MonoObject*>* csTriggerOut;

	void PopulateMetadatas() override;

	void InvalidateAssembly() { m_scriptingAssembly = nullptr; }

	std::unordered_map<std::string, std::shared_ptr<class CCProperty::IClearProperty>> m_properties;
	std::unordered_map<std::string, std::shared_ptr<class Field>> m_field;

public:
	ScriptedBehaviour();
	ScriptedBehaviour(CCUUID& id);
	virtual ~ScriptedBehaviour();

	void SetScriptClass(const std::string& scriptName);
	void BindToSignals() override;
	void OnSetOwner(Entity* newOwner)  override;
	void Awake();
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