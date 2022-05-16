#pragma once

#include <cherry_macros.hpp>

#include "object.hpp"

#include "metadata.hpp"
#include "scene.hpp"
#include "serializer.hpp"

class CCENGINE_API Behaviour : public Object
{
	friend class Scene;
	friend class Serializer;
private:
protected:
	virtual void PopulateMetadatas()
	{
		m_metadatas.SetProperty("m_owner", &owner);
	}

public:
	class Entity* m_owner = nullptr;

	Event<> m_OnDestroy;

	Behaviour() = default;
	Behaviour(CCUUID& id) : Object(id) {}
	virtual ~Behaviour() { m_OnDestroy.Invoke(); }

	virtual void LateUpdate() {}
	virtual void FixedUpdate() {}
	virtual void OnEnable() {}
	virtual void OnDisable() {}
	virtual void BindToSignals() {}

	std::unordered_map<std::string, Field>& GetFields() { return m_metadatas.m_fields; }
	std::unordered_map<std::string, CCProperty::IClearProperty*>& GetProperties() { return m_metadatas.m_properties; }

	Entity& GetHost() { return *m_owner; }
	const Entity& GetHost() const { return *m_owner; }

	Entity* GetHostPtr() { return m_owner; }
	void SetHostPtr(Entity* newOwner);
	CCProperty::CopyProperty<Behaviour, Entity*> owner{ this, &Behaviour::SetHostPtr, &Behaviour::GetHostPtr};
};
