#pragma once

#include "cherry_macros.hpp"

#include "behaviour.hpp"

class Transform;
namespace physx
{
	class PxShape;
}

namespace PhysicSystem
{
	class PhysicActor;
}

enum class EColliderShape
{
	NONE,
	BOX_COLLIDER,
	SPHERE_COLLIDER,
};

class CCENGINE_API Collider : public Behaviour
{
private:
	EColliderShape	m_colliderShape;
	physx::PxShape* m_pxShape;
	
	bool	m_isEnabled = true;
	bool	m_isTrigger = false;
	float	m_contactOffset = 2.f;

	void PopulateMetadatas() override;

public:
	Entity& m_owner;

	PhysicSystem::PhysicActor* m_physicActor = nullptr;

	Collider(Entity& owner);

	void SetPxShape();
	void SetPxData();

	EColliderShape& GetShape() { return m_colliderShape; }
};
