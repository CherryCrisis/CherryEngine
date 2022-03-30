#pragma once

#include "cherry_macros.hpp"

#include "behaviour.hpp"
#include "maths.hpp"

class Transform;
namespace PhysicSystem
{
	class PhysicActor;
}

using namespace CCMaths;

class CCENGINE_API Rigidbody : public Behaviour
{
private:
	float	m_density = 10.0f;

	bool	m_isEnabled;
	bool	m_isKinematic = false;
	bool	m_useGravity = true;

	bool	m_positionConstraints[3] = {};
	bool	m_rotationConstraints[3] = {};

	Vector3 m_velocity			= Vector3::Zero;
	Vector3 m_angularVelocity	= Vector3::Zero;
	Vector3 m_InertiaTensor		= Vector3::Zero;

	float m_maxLinearVelocity			= (float)1.00000003e+16;
	float m_maxAngularVelocity			= 100.0f;
	float m_maxDepenetrationVelocity	= (float)1.00000003e+32;

	void PopulateMetadatas() override;

public:
	Entity& m_owner;
	
	PhysicSystem::PhysicActor* m_physicActor = nullptr;
	
	Rigidbody(Entity& owner);

	void SetPxActor();
	void SetActorConstraints();
	void SetActorEnabled();
	void SetActorKinetic();
	void SetActorGravity();
	void SetActorMaxVelocities();
	void SetActorDensity();
};