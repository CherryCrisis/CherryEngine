#include <pch.hpp>

#include "character_controller.hpp"

#include <PxPhysicsAPI.h>

#include "input_manager.hpp"
#include "physic_manager.hpp"

#include "capsule_collider.hpp"
#include "physic_actor.hpp"
#include "rigidbody.hpp"
#include "transform.hpp"

CharacterController::CharacterController()
{
	PopulateMetadatas();
}

CharacterController::CharacterController(CCUUID& id) : Behaviour(id)
{
	PopulateMetadatas();
}

CharacterController::~CharacterController()
{
	Unregister();

	if (!m_isStarted)
		GetHost().m_OnStart.Unbind(&CharacterController::Initialize, this);

	GetHost().m_OnTick.Unbind(&CharacterController::Update, this);
	GetHost().m_OnFixedTick.Unbind(&CharacterController::FixedUpdate, this);

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
	GetHost().m_OnTick.Bind(&CharacterController::Update, this);
	GetHost().m_OnFixedTick.Bind(&CharacterController::FixedUpdate, this);
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
		m_rigidbody = m_physicActor->GetRigidbody();

	if (m_physicActor->Get())
		m_dynamicActor = static_cast<physx::PxRigidDynamic*>(m_physicActor->Get());

	m_isStarted = true;
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

void CharacterController::Update()
{
	if (!m_transform || !m_collider || !m_rigidbody || !m_physicActor || !m_dynamicActor)
		return;

	InputManager::PushContext("User Context");

	m_forwardMove = InputManager::GetAxis(Keycode::W, Keycode::S);
	m_sideMove = InputManager::GetAxis(Keycode::D, Keycode::A);

	if (InputManager::GetKey(Keycode::SPACE) && m_isGrounded)
		m_physicActor->AddForce({ 0, 10.f, 0 }, PhysicSystem::EForceMode::eIMPULSE);

	m_rotating = CCMaths::Vector3::YAxis * InputManager::GetMouseDelta().x;
	
	InputManager::PopContext();
}

void CharacterController::FixedUpdate()
{
	if (!m_transform || !m_collider || !m_rigidbody || !m_physicActor || !m_dynamicActor)
		return;

	float raycastDist = m_collider->GetScale() + m_collider->GetRadius() + m_contactOffset;

	PhysicSystem::RaycastHit hit = m_physicActor->Raycast(m_transform->GetGlobalPosition(), { 0, -1, 0 }, raycastDist);

	physx::PxVec3 pxVel = m_dynamicActor->getLinearVelocity();
	CCMaths::Vector3 vel = { pxVel.x, pxVel.y, pxVel.z };

	physx::PxVec3 pxRVel = m_dynamicActor->getAngularVelocity();
	CCMaths::Vector3 rVel = { pxRVel.x, pxRVel.y, pxRVel.z };

	if (hit.actor)
	{
		m_isGrounded = true;

		float deltaDistance = hit.distance - raycastDist;
		float downVel = CCMaths::Vector3::Dot({ 0, -1, 0 }, vel);

		float force = deltaDistance * m_springStrength - downVel * m_springDampling;

		m_physicActor->AddForce({ 0, -force, 0 }, PhysicSystem::EForceMode::eFORCE);

	}
	else
	{
		m_isGrounded = false;
	}

	CCMaths::Vector3 move = -m_transform->GetWorldMatrix().back.Normalized() * m_forwardMove + m_transform->GetWorldMatrix().right.Normalized() * m_sideMove;
	CCMaths::Vector3 goalVelocity = move * m_moveSpeed;
	CCMaths::Vector3 neededAcceleration = CCMaths::Vector3::ClampLength((goalVelocity - vel) / TimeManager::GetFixedDeltaTime(), -150.f, 150.f);
	CCMaths::Vector3 neededForce = CCMaths::Vector3::Multiply(neededAcceleration * m_dynamicActor->getMass(), { 1, 0, 1 });

	m_physicActor->AddForce(neededForce, PhysicSystem::EForceMode::eFORCE);

	CCMaths::Vector3 rot = m_rotating * TimeManager::GetFixedDeltaTime();
	CCMaths::Vector3 goalRVelocity = rot * m_sensitivity;
	CCMaths::Vector3 neededRAcceleration = CCMaths::Vector3::ClampLength((goalRVelocity - rVel) / TimeManager::GetFixedDeltaTime(), -150.f, 150.f);

	m_physicActor->AddTorque(neededRAcceleration, PhysicSystem::EForceMode::eFORCE);
}

void CharacterController::InvalidateTransform()
{
	m_transform = nullptr;
}