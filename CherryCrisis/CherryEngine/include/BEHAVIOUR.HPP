#pragma once

#include <cherry_macros.hpp>

#include "component.hpp"
#include "entity.hpp"

#include <iostream>

class CCENGINE_API Behaviour : public Component
{
	friend class Entity;

private:
	Entity& m_owner;

public:
	Behaviour(Entity& owner) : m_owner(owner) { }
	virtual ~Behaviour() = default;

	virtual void Awake() {}
	virtual void Start() {}
	virtual void Update() {}
	virtual void LateUpdate() {}
	virtual void FixedUpdate() {}
	virtual void OnEnable() {}
	virtual void OnDisable() {}

	Entity& GetHost() { return m_owner; }
	const Entity& GetHost() const { return m_owner; }
};