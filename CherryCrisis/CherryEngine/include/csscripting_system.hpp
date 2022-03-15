#pragma once

#include "singleton.hpp"

#include "monowrapper.hpp"

class CsScriptingSystem : public Singleton<CsScriptingSystem>
{
	friend class Singleton<CsScriptingSystem>;

public:
	std::unique_ptr<mono::ManagedScriptSystem> m_scriptSystem;

	CsScriptingSystem();

	void Init(const mono::ManagedScriptSystemSettings_t& settings);
};