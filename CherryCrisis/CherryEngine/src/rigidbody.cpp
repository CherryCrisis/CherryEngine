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
void Rigidbody::SetEnabled(const bool& isEnabled)
{
	if (m_physicActor)
		m_physicActor->SetEnabled(isEnabled);
}

bool Rigidbody::GetEnabled()
{
	return m_physicActor->GetEnabled();
}

void Rigidbody::Rigidbody::SetKinematic(const bool& isKinematic)
{
	if (m_physicActor)
		m_physicActor->SetKinematic(isKinematic);
}

bool Rigidbody::Rigidbody::GetKinematic()
{
	return m_physicActor->GetKinematic();
}

void Rigidbody::SetGravity(const bool& useGravity)
{
	if (m_physicActor)
		m_physicActor->SetGravity(useGravity);
}

bool Rigidbody::GetGravity()
{
	return m_physicActor->GetGravity();
}
	 
void Rigidbody::SetPosContraints(const Bool3& constraints)
{
	if (m_physicActor)
		m_physicActor->SetPosContraints(constraints);
}

Bool3 Rigidbody::GetPosConstraints()
{
	return m_physicActor->GetPosConstraints();
}

void Rigidbody::SetRotContraints(const Bool3& constraints)
{
	if (m_physicActor)
		m_physicActor->SetRotContraints(constraints);
}

Bool3 Rigidbody::GetRotConstraints()
{
	return m_physicActor->GetRotConstraints();
}

void Rigidbody::SetDensity(const float& density)
{
	if (m_physicActor)
		m_physicActor->SetDensity(density);
}

float Rigidbody::GetDensity()
{
	return m_physicActor->GetDensity();
}

void Rigidbody::SetMaxVel(const float& velocity)
{
	if (m_physicActor)
		m_physicActor->SetMaxVel(velocity);
}

float Rigidbody::GetMaxVel()
{
	return m_physicActor->GetMaxVel();
}

void Rigidbody::SetMaxAngVel(const float& velocity)
{
	if (m_physicActor)
		m_physicActor->SetMaxAngVel(velocity);
}

float Rigidbody::GetMaxAngVel()
{
	return m_physicActor->GetMaxAngVel();
}

void Rigidbody::SetMaxDepVel(const float& velocity)
{
	if (m_physicActor)
		m_physicActor->SetMaxDepVel(velocity);
}

float Rigidbody::GetMaxDepVel()
{
	return m_physicActor->GetMaxDepVel();
}
#pragma endregion