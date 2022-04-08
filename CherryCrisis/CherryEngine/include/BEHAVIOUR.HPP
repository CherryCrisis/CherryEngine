#pragma once

#include <cherry_macros.hpp>

#include "component.hpp"
#include "entity.hpp"

#include "metadata.hpp"

class CCENGINE_API Behaviour : public Component
{
	friend class Entity;
	friend class Scene;

private:
	Entity* m_owner = nullptr;
	CCUUID m_uuid = {};
	CCUUID m_parentUuid = {};
protected:
	Metadata m_metadatas;

	virtual void PopulateMetadatas() {}
public:
	Behaviour() = default;
	Behaviour(CCUUID& owner) { m_uuid = owner; }
	virtual ~Behaviour() = default;

	virtual void LateUpdate() {}
	virtual void FixedUpdate() {}
	virtual void OnEnable() {}
	virtual void OnDisable() {}
	virtual void BindToSignals() {}
	std::string Serialize();

	std::unordered_map<std::string, Field>& GetFields() { return m_metadatas.m_fields; }
	std::unordered_map<std::string, CCProperty::IClearProperty*>& GetProperties() { return m_metadatas.m_properties; }

	Entity& GetHost() { return *m_owner; }
	const Entity& GetHost() const { return *m_owner; }
	const uint32_t GetUUID() const { return (uint32_t)m_uuid; }
	const uint32_t GetOwnerUUID() const { return (uint32_t)m_parentUuid; }
};
