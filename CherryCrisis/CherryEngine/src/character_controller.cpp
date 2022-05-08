#include "pch.hpp"

#include "character_controller.hpp"

#include "input_manager.hpp"
#include "physic_manager.hpp"
#include "transform.hpp"
#include "capsule_collider.hpp"

CharacterController::CharacterController(CCUUID& id) : Behaviour(id)
{
	PopulateMetadatas();
}

CharacterController::~CharacterController()
{
	Unregister();
}

void CharacterController::PopulateMetadatas()
{
	Behaviour::PopulateMetadatas();

	m_metadatas.SetProperty("m_contactOffset",	&contactOffset);
}

void CharacterController::BindToSignals()
{
	PhysicSystem::PhysicManager* physicManager = PhysicSystem::PhysicManager::GetInstance();

	// TODO: Implement function in PhysicManager
	//physicManager->Register(this);
	m_isRegistered = true;
}

void CharacterController::Unregister()
{
	if (m_isRegistered)
	{
		PhysicSystem::PhysicManager* physicManager = PhysicSystem::PhysicManager::GetInstance();

		// TODO: Implement function in PhysicManager
		//physicManager->Unregister(this);
		m_isRegistered = false;
	}
}

// TODO: Verify if Entity has collider and rigidbody on launch else create them

void CharacterController::Update()
{
	Transform* transform = GetHost().GetOrAddBehaviour<Transform>();
	CapsuleCollider* collider = GetHost().GetOrAddBehaviour<CapsuleCollider>();

	float raycastDist = collider->GetScale() + collider->GetRadius() + m_contactOffset;

	PhysicSystem::RaycastHit hit = m_physicActor->Raycast(transform->GetPosition(), {0, -1, 0}, raycastDist);

	if (hit.actor)
	{
		m_physicActor->AddForce({0, 0.1f, 0}, PhysicSystem::EForceMode::eIMPULSE);
	}

	InputManager* IM = InputManager::GetInstance();

	IM->SetPollContext("User Context");

	float forward = IM->GetAxis("Horizontal");

	m_physicActor->AddForce(transform->GetWorldMatrix().back * forward, PhysicSystem::EForceMode::eACCELERATION);
}
