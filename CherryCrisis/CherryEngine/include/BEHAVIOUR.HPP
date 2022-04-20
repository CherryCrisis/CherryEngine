#pragma once

#include <cherry_macros.hpp>

#include "object.hpp"
#include "entity.hpp"

#include "metadata.hpp"
#include "scene.hpp"
#include "serializer.hpp"

class CCENGINE_API Behaviour : public Object
{
	friend class Entity;
	friend class Scene;
	friend class Serializer;
private:
	Entity* m_owner = nullptr;
protected:
	Metadata m_metadatas;

	virtual void PopulateMetadatas()
	{
		m_metadatas.SetField<Object*>("owner", m_owner);
	}
public:
	Behaviour() = default;
	Behaviour(CCUUID& owner) : Object(owner) {}
	virtual ~Behaviour() = default;

	virtual void LateUpdate() {}
	virtual void FixedUpdate() {}
	virtual void OnEnable() {}
	virtual void OnDisable() {}
	virtual void BindToSignals() {}

	std::unordered_map<std::string, Field>& GetFields() { return m_metadatas.m_fields; }
	std::unordered_map<std::string, CCProperty::IClearProperty*>& GetProperties() { return m_metadatas.m_properties; }

	Entity& GetHost() { return *m_owner; }
	const Entity& GetHost() const { return *m_owner; }
};
