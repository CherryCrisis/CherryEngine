#include <pch.hpp>

#include "rigidbody.hpp"

#include <PxPhysicsAPI.h>

#include "physic_manager.hpp"

#include "transform.hpp"
#include "entity.hpp"

Rigidbody::Rigidbody()
{
	PopulateMetadatas();
}

Rigidbody::Rigidbody(CCUUID& id) : Behaviour(id)
{
	PopulateMetadatas();
}

Rigidbody::~Rigidbody()
{
	Unregister();
}

void Rigidbody::BindToSignals()
{
	PhysicSystem::PhysicManager::Register(this);
	m_isRegistered = true;

	GetHost().m_OnAwake.Bind(&Rigidbody::Initialize, this);
	GetHost().m_OnStart.Bind(&Rigidbody::Start, this);
}

void Rigidbody::Initialize()
{
	m_physicActor->Init();

	GetHost().m_OnAwake.Unbind(&Rigidbody::Initialize, this);
}

void Rigidbody::Start()
{
	if (m_physicActor->Get())
		m_dynamicActor = static_cast<physx::PxRigidDynamic*>(m_physicActor->Get());

	m_transform = GetHost().GetOrAddBehaviour<Transform>();

	GetHost().m_OnStart.Unbind(&Rigidbody::Start, this);
}

void Rigidbody::Unregister()
{
	if (m_isRegistered)
	{
		PhysicSystem::PhysicManager::Unregister(this);
		m_isRegistered = false;
	}
}

void Rigidbody::PopulateMetadatas()
{
	Behaviour::PopulateMetadatas();

	m_metadatas.SetProperty("Density", &density);
	m_metadatas.SetProperty("Enabled", &isEnabled);
	m_metadatas.SetProperty("Kinematic", &isKinematic);
	m_metadatas.SetProperty("Use gravity", &useGravity);

	m_metadatas.SetProperty("Position constraints", &positionConstraints);
	m_metadatas.SetProperty("Rotation constraints", &rotationConstraints);

	m_metadatas.SetProperty("Max velocity", &maxLinearVelocity);
	m_metadatas.SetProperty("Max angular velocity", &maxAngularVelocity);
	m_metadatas.SetProperty("Max depenetration velocity", &maxDepenetrationVelocity);
}

#pragma region Set&Get
void Rigidbody::SetEnabled()
{
	if (m_physicActor)
		m_physicActor->SetEnabled(m_isEnabled);
}

void Rigidbody::SetVelocity(const Vector3& vel) 
{
	m_dynamicActor->setLinearVelocity({vel.x, vel.y, vel.z});
	m_dynamicActor->setAngularVelocity({ vel.x, vel.y, vel.z });
}

void Rigidbody::SaveVelocity()
{
	physx::PxVec3 pxVelocity = m_dynamicActor->getLinearVelocity();
	Vector3 velocity = { pxVelocity.x, pxVelocity.y, pxVelocity.z };

	m_SavedVelocity.x = Vector3::Dot(m_transform->Right(), velocity);
	m_SavedVelocity.y = Vector3::Dot(m_transform->Up(), velocity);
	m_SavedVelocity.z = Vector3::Dot(m_transform->Forward(), velocity);

	SetVelocity(Vector3::Zero);
}

void Rigidbody::ReapplyVelocity()
{
	// To recompute world matrix and Right, Forwrd and Up vectors
	m_transform->GetWorldMatrix();

	Vector3 velocity =	m_transform->Right() * m_SavedVelocity.x +
						m_transform->Up() * m_SavedVelocity.y + 
						m_transform->Forward() * m_SavedVelocity.z;

	physx::PxVec3 pxVelocity = { velocity.x, velocity.y, velocity.z };

	m_dynamicActor->setLinearVelocity(pxVelocity);

	m_SavedVelocity = Vector3::Zero;
}

void Rigidbody::SetEnabled(const bool& isEnabled)
{
	m_isEnabled = isEnabled;

	if (m_physicActor)
		m_physicActor->SetEnabled(isEnabled);
}

bool Rigidbody::GetEnabled()
{
	return m_isEnabled;
}

void Rigidbody::Rigidbody::SetKinematic(const bool& isKinematic)
{
	m_isKinematic = isKinematic;

	if (m_physicActor)
		m_physicActor->SetActorKinematic();
}

bool Rigidbody::Rigidbody::GetKinematic()
{
	return m_isKinematic;
}

void Rigidbody::SetGravity(const bool& useGravity)
{
	m_useGravity = useGravity;

	if (m_physicActor)
		m_physicActor->SetActorGravity();
}

bool Rigidbody::GetGravity()
{
	return m_useGravity;
}
	 
void Rigidbody::SetPosContraints(const Bool3& constraints)
{
	m_positionConstraints = constraints;

	if (m_physicActor)
		m_physicActor->SetActorConstraints();
}

Bool3 Rigidbody::GetPosConstraints()
{
	return m_positionConstraints;
}

void Rigidbody::SetRotContraints(const Bool3& constraints)
{
	m_rotationConstraints = constraints;

	if (m_physicActor)
		m_physicActor->SetActorConstraints();
}

Bool3 Rigidbody::GetRotConstraints()
{
	return m_rotationConstraints;
}

void Rigidbody::SetDensity(const float& density)
{
	m_density = density;

	if (m_physicActor)
		m_physicActor->SetActorDensity();
}

float Rigidbody::GetDensity()
{
	return m_density;
}

void Rigidbody::SetMaxVel(const float& velocity)
{
	m_maxLinearVelocity = velocity;

	if (m_physicActor)
		m_physicActor->SetActorMaxVelocities();
}

float Rigidbody::GetMaxVel()
{
	return m_maxLinearVelocity;
}

void Rigidbody::SetMaxAngVel(const float& velocity)
{
	m_maxAngularVelocity = velocity;

	if (m_physicActor)
		m_physicActor->SetActorMaxVelocities();
}

float Rigidbody::GetMaxAngVel()
{
	return m_maxAngularVelocity;
}

void Rigidbody::SetMaxDepVel(const float& velocity)
{
	m_maxDepenetrationVelocity = velocity;

	if (m_physicActor)
		m_physicActor->SetActorMaxVelocities();
}

float Rigidbody::GetMaxDepVel()
{
	return m_maxDepenetrationVelocity;
}
#pragma endregion

void Rigidbody::AddForce(const CCMaths::Vector3& force, PhysicSystem::EForceMode mode)
{
	if (m_physicActor)
		m_physicActor->AddForce(force, mode);
}

void Rigidbody::AddTorque(const CCMaths::Vector3& force, PhysicSystem::EForceMode mode)
{
	if (m_physicActor)
		m_physicActor->AddTorque(force, mode);
}