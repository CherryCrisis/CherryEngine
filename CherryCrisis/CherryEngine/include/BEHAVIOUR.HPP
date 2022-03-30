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
	virtual void ConsumeMetadatas()  {}
public:
	Behaviour() = default;
	Behaviour(Entity& owner) : m_owner(&owner) { }
	Behaviour(CCUUID& owner) { m_uuid = owner; }
	virtual ~Behaviour() = default;

	virtual void Awake() {}
	virtual void Start() {}
	virtual void Update() {}
	virtual void LateUpdate() {}
	virtual void FixedUpdate() {}
	virtual void OnEnable() {}
	virtual void OnDisable() {}
	std::string Serialize();

	std::unordered_map<std::string, Field>& GetFields() { return m_metadatas.m_fields; }
	std::unordered_map<std::string, CCProperty::IClearProperty*>& GetProperties() { return m_metadatas.m_properties; }

	Entity& GetHost() { return *m_owner; }
	const Entity& GetHost() const { return *m_owner; }
	const uint64_t GetUUID() const { return (uint64_t)m_uuid; }
	const uint64_t GetOwnerUUID() const { return (uint64_t)m_parentUuid; }
};
