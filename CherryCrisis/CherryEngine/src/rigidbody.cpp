#include <pch.hpp>

#include "rigidbody.hpp"

#include <PxPhysicsAPI.h>

#include "physic_manager.hpp"

#include "transform.hpp"


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
	PhysicSystem::PhysicManager* physicManager = PhysicSystem::PhysicManager::GetInstance();
	
	physicManager->Register(this);
	m_isRegistered = true;

	GetHost().m_OnAwake.Bind(&Rigidbody::Initialize, this);
}

void Rigidbody::Initialize()
{
	m_physicActor->Init();

	GetHost().m_OnAwake.Unbind(&Rigidbody::Initialize, this);
}

void Rigidbody::Unregister()
{
	if (m_isRegistered)
	{
		PhysicSystem::PhysicManager* physicManager = PhysicSystem::PhysicManager::GetInstance();

		physicManager->Unregister(this);
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
void Rigidbody::Rigidbody::SetEnabled()
{
	if (m_physicActor)
		m_physicActor->SetEnabled(m_isEnabled);
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