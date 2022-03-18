#pragma once

#include <cherry_macros.hpp>

#include "component.hpp"
#include "entity.hpp"

#include "metadata.hpp"

class CCENGINE_API Behaviour : public Component
{
	friend class Entity;

private:
	Entity* m_owner = nullptr;

protected:
	Metadata m_metadatas;

	virtual void PopulateMetadatas() {}

public:
	Behaviour() = default;
	Behaviour(Entity& owner) : m_owner(&owner) { }
	virtual ~Behaviour() = default;

	virtual void Awake() {}
	virtual void Start() {}
	virtual void Update() {}
	virtual void LateUpdate() {}
	virtual void FixedUpdate() {}
	virtual void OnEnable() {}
	virtual void OnDisable() {}

	std::vector<Field>& GetFields() { return m_metadatas.m_fields; }

	Entity& GetHost() { return *m_owner; }
	const Entity& GetHost() const { return *m_owner; }
};