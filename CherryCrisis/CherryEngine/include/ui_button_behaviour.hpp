#pragma once

#include <functional>
#include <memory>

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

class CCENGINE_API ButtonBehaviour
{
	std::string m_scriptName;

	bool m_linked = false;

	std::shared_ptr<class CsAssembly> m_assembly;
	mono::ManagedScriptSystem* script;
	mono::ManagedObject* managedInstance;
	mono::ManagedClass* managedClass;
	mono::ManagedMethod* managedOnClick;

	mono::ManagedThunk<void, struct _MonoObject*>* csOnClick;

	void InvalidateAssembly() { m_assembly = nullptr; }

public:
	ButtonBehaviour();
	ButtonBehaviour(CCUUID& id);
	~ButtonBehaviour();

	void SetScriptClass(const std::string& scriptName);

	void Reload(std::shared_ptr<CsAssembly> csAssembly);

	void OnClick();

	_MonoObject* GetRawInstance();
};