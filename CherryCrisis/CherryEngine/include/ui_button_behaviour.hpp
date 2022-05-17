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

class CCENGINE_API ButtonBehaviour : public Behaviour
{
	std::string m_scriptName;

	bool m_linked = false;

	std::shared_ptr<class CsAssembly> m_assembly;
	mono::ManagedScriptSystem* script;
	mono::ManagedObject* managedInstance;
	mono::ManagedClass* managedClass;
	mono::ManagedMethod* managedOnClick;

	mono::ManagedThunk<void, struct _MonoObject*>* csOnClick;

	void PopulateMetadatas() override;

	void InvalidateAssembly() { m_assembly = nullptr; }

public:
	ButtonBehaviour();
	ButtonBehaviour(CCUUID& id);
	~ButtonBehaviour();

	void SetScriptClass(const std::string& scriptName);

	void Reload(std::shared_ptr<CsAssembly> csAssembly);

	void OnClick();
	std::string GetScriptPath() { return m_scriptName; }

	_MonoObject* GetRawInstance();

	CCProperty::ConstRefProperty<ButtonBehaviour, std::string> scriptPath{ this, &ButtonBehaviour::SetScriptClass, &ButtonBehaviour::GetScriptPath };
};