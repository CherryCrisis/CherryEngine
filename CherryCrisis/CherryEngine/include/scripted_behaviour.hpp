#pragma once

#include <functional>
#include <memory>
#include <unordered_map>

#include <cherry_macros.hpp>

#include "behaviour.hpp"

namespace mono
{
	class ManagedClass;
	class ManagedMethod;
	class ManagedObject;
	class ManagedScriptSystem;
	template <typename RetT, typename ...Args>
	class ManagedThunk;
}

struct _MonoException;


class CCENGINE_API ScriptedBehaviour : public Behaviour
{
	bool m_linked = false;
	bool m_subscribed = false;

	std::string m_scriptName;

	std::unordered_map<std::string, std::shared_ptr<struct CCProperty::IClearProperty>> m_properties;
	std::unordered_map<std::string, std::shared_ptr<struct Field>>						m_field;
	std::unordered_map<Entity*, mono::ManagedObject*>									m_physicEntities;

	std::shared_ptr<class CsAssembly>	m_interfaceAssembly;
	std::shared_ptr<class CsAssembly>	m_scriptingAssembly;

	mono::ManagedObject*				m_managedInstance = nullptr;
	mono::ManagedMethod*				m_managedAwake = nullptr;
	mono::ManagedClass*					m_managedClass = nullptr;
	mono::ManagedMethod*				m_managedUpdate = nullptr;
	mono::ManagedMethod*				m_managedStart = nullptr;
	mono::ManagedMethod*				m_managedCollideIn = nullptr;
	mono::ManagedMethod*				m_managedCollideStay = nullptr;
	mono::ManagedMethod*				m_managedCollideOut = nullptr;
	mono::ManagedMethod*				m_managedTriggerIn = nullptr;
	mono::ManagedMethod*				m_managedTriggerOut = nullptr;

	mono::ManagedThunk<void, struct _MonoObject*>* csAwake;
	mono::ManagedThunk<void, struct _MonoObject*>* csStart;
	mono::ManagedThunk<void, struct _MonoObject*>* csUpdate;

	mono::ManagedThunk<void, struct _MonoObject*, struct _MonoObject*>* csCollideIn;
	mono::ManagedThunk<void, struct _MonoObject*, struct _MonoObject*>* csCollideStay;
	mono::ManagedThunk<void, struct _MonoObject*, struct _MonoObject*>* csCollideOut;
	mono::ManagedThunk<void, struct _MonoObject*, struct _MonoObject*>* csTriggerIn;
	mono::ManagedThunk<void, struct _MonoObject*, struct _MonoObject*>* csTriggerOut;

	mono::ManagedClass* m_behaviourClass = nullptr;
	mono::ManagedClass* m_entityClass = nullptr;

	void PopulateMetadatas() override;

	void InvalidateAssembly() { m_scriptingAssembly = nullptr; }

public:
	ScriptedBehaviour();
	ScriptedBehaviour(CCUUID& id);
	virtual ~ScriptedBehaviour();

	const std::string TypeName() override { return m_scriptName; }
	void SetScriptClass(const std::string& scriptName);
	void OnSetOwner(Entity* newOwner) override;
	void Awake();
	void Start();
	void Update();

	void SetSignals();
	void UnsetSignals();

	void OnCollisionEnter(Entity* other);
	void OnCollisionStay(Entity* other);
	void OnCollisionExit(Entity* other);
	void OnTriggerEnter(Entity* other);
	void OnTriggerExit(Entity* other);

	void Reload(std::shared_ptr<CsAssembly> csAssembly);

	std::string GetScriptPath() { return m_scriptName; }

	_MonoObject* GetRawInstance();

	CCProperty::ConstRefProperty<ScriptedBehaviour, std::string> scriptPath { this, &ScriptedBehaviour::SetScriptClass, &ScriptedBehaviour::GetScriptPath};
};