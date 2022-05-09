#include "pch.hpp"

#include "character_controller.hpp"

#include <PxPhysicsAPI.h>

#include "input_manager.hpp"
#include "physic_manager.hpp"
#include "physic_actor.hpp"
#include "rigidbody.hpp"
#include "transform.hpp"
#include "capsule_collider.hpp"

CharacterController::CharacterController(CCUUID& id) : Behaviour(id)
{
	PopulateMetadatas();
}

CharacterController::~CharacterController()
{
	Unregister();
	
	if (m_transform)
	{
		m_transform->m_OnDestroy.Unbind(&CharacterController::InvalidateTransform, this);
	}
}

void CharacterController::PopulateMetadatas()
{
	Behaviour::PopulateMetadatas();

	m_metadatas.SetProperty("Move Speed", &moveSpeed);
	m_metadatas.SetProperty("Spring Dampling", &springDampling);
	m_metadatas.SetProperty("String Strength", &springStrength);
}

void CharacterController::BindToSignals()
{
	GetHost().m_OnStart.Bind(&CharacterController::Initialize, this);

	PhysicSystem::PhysicManager* physicManager = PhysicSystem::PhysicManager::GetInstance();

	physicManager->Register(this);
	m_isRegistered = true;
}

void CharacterController::Initialize()
{
	Entity& owner = GetHost();

	m_transform = owner.GetOrAddBehaviour<Transform>();

	if (m_transform)
		m_transform->m_OnDestroy.Bind(&CharacterController::InvalidateTransform, this);

	m_collider = owner.GetOrAddBehaviour<CapsuleCollider>();

	if (!owner.HasBehaviour<Rigidbody>())
	{
		m_rigidbody = owner.AddBehaviour<Rigidbody>();

		m_rigidbody->SetGravity(true);
		m_rigidbody->SetKinematic(false);
		m_rigidbody->SetRotContraints({ true, false, true });

		if (m_physicActor->Get())
		{
			m_physicActor->DestroyPxActor();
			m_physicActor->CreatePxActor();
		}
	}
	else
		// change with physicActor rigidbody to be sure if there are multiple on the entity
		m_rigidbody = owner.GetBehaviour<Rigidbody>();

	GetHost().m_OnStart.Unbind(&CharacterController::Initialize, this);
}

void CharacterController::Unregister()
{
	if (m_isRegistered)
	{
		PhysicSystem::PhysicManager* physicManager = PhysicSystem::PhysicManager::GetInstance();

		physicManager->Unregister(this);
		m_isRegistered = false;
	}
}

// TODO: Verify if Entity has collider and rigidbody on launch else create them

void CharacterController::Update()
{
	if (!m_transform || !m_collider || !m_rigidbody)
		return;

	float raycastDist = m_collider->GetScale() + m_collider->GetRadius() + m_contactOffset;

	PhysicSystem::RaycastHit hit = m_physicActor->Raycast(m_transform->GetPosition(), {0, -1, 0}, raycastDist);

	physx::PxRigidDynamic* actor = static_cast<physx::PxRigidDynamic*>(m_physicActor->Get());
	physx::PxVec3 pxVel = actor->getLinearVelocity();
	CCMaths::Vector3 vel = { pxVel.x, pxVel.y, pxVel.z };

	if (hit.actor)
	{
		m_isGrounded = true;

		float deltaDistance = hit.distance - raycastDist;
		float downVel = CCMaths::Vector3::Dot({ 0, -1, 0 }, vel);
	
		float force = deltaDistance * m_springStrength - downVel * m_springDampling;

		m_physicActor->AddForce({0, -force, 0}, PhysicSystem::EForceMode::eFORCE);

	}
	else
	{
		m_isGrounded = false;
	}

	InputManager* IM = InputManager::GetInstance();

	IM->PushContext("User Context");

	float forward = IM->GetAxis(Keycode::W, Keycode::S);
	float side = IM->GetAxis(Keycode::D, Keycode::A);

	CCMaths::Vector3 rot = CCMaths::Vector3::YAxis * IM->GetMouseDelta().x / 60.f /* detlaTime */;

	if (IM->GetKey(Keycode::SPACE) && m_isGrounded)
		m_physicActor->AddForce({0, 10.f, 0}, PhysicSystem::EForceMode::eIMPULSE);

	IM->PopContext();

	CCMaths::Vector3 move = -m_transform->GetWorldMatrix().back.Normalized() * forward + m_transform->GetWorldMatrix().right.Normalized() * side;

	CCMaths::Vector3 goalVelocity = move * m_moveSpeed;

	CCMaths::Vector3 neededAcceleration = CCMaths::Vector3::ClampLength((goalVelocity - vel) * 60.f /* detlaTime */, -150.f, 150.f);
	CCMaths::Vector3 neededForce = CCMaths::Vector3::Multiply(neededAcceleration * actor->getMass(), { 1, 0, 1 });

	m_physicActor->AddForce(neededForce, PhysicSystem::EForceMode::eFORCE);
	m_transform->SetRotation(m_transform->GetRotation() + rot);
}

void CharacterController::InvalidateTransform()
{
	m_transform = nullptr;
}