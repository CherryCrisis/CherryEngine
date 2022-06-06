#include <pch.hpp>

#include "character_controller.hpp"

#include <PxPhysicsAPI.h>

#include "input_manager.hpp"
#include "physic_manager.hpp"

#include "capsule_collider.hpp"
#include "physic_actor.hpp"
#include "rigidbody.hpp"
#include "transform.hpp"
#include "entity.hpp"

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

	m_metadatas.SetProperty("Walking Speed", &walkingSpeed);
	m_metadatas.SetProperty("Running Speed", &runningSpeed);
	m_metadatas.SetProperty("Spring Dampling", &springDampling);
	m_metadatas.SetProperty("String Strength", &springStrength);
	m_metadatas.SetProperty("Rotation Sensivity", &sensitivity);
	m_metadatas.SetProperty("Jump Force", &jumpForce);
}

void CharacterController::BindToSignals()
{
	GetHost().m_OnTick.Bind(&CharacterController::Update, this);
	GetHost().m_OnFixedTick.Bind(&CharacterController::FixedUpdate, this);
	GetHost().m_OnStart.Bind(&CharacterController::Initialize, this);

	PhysicSystem::PhysicManager::Register(this);
	m_isRegistered = true;
}

void CharacterController::Initialize()
{
	Entity& owner = GetHost();

	m_transform = owner.GetOrAddBehaviour<Transform>();

	if (m_transform)
		m_transform->m_OnDestroy.Bind(&CharacterController::InvalidateTransform, this);

	m_physicActor->Init();

	m_collider = owner.GetOrAddBehaviour<CapsuleCollider>();

	if (!owner.HasBehaviour<Rigidbody>())
	{
		m_rigidbody = owner.AddBehaviour<Rigidbody>();

		m_rigidbody->SetGravity(true);
		m_rigidbody->SetKinematic(false);
		m_rigidbody->SetRotContraints({ true, true, true });

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
	owner.m_OnStart.Unbind(&CharacterController::Initialize, this);
}

void CharacterController::Unregister()
{
	if (m_isRegistered)
	{
		PhysicSystem::PhysicManager::Unregister(this);
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
	{
		m_physicActor->AddForce({ 0, m_jumpForce, 0 }, PhysicSystem::EForceMode::eIMPULSE);
		m_isGrounded = false;
	}
	m_isRunning = InputManager::GetKey(Keycode::LEFT_SHIFT);

	if (!m_alignement.m_isRotating)
	{
		m_rotating = InputManager::GetMouseDelta().x;
		CCMaths::Vector3 rot = CCMaths::Vector3::YAxis * m_rotating * m_sensitivity * 0.001f; // 0.001f to keep sensitivity value above 1
		m_transform->SetRotation((m_transform->GetRotation() * Quaternion::FromEuler(rot)).Normalized());
	}

	InputManager::PopContext();
}

void CharacterController::FixedUpdate()
{
	if (!m_transform || !m_collider || !m_rigidbody || !m_physicActor || !m_dynamicActor)
		return;

	float raycastDist = m_collider->GetScale() + m_collider->GetRadius() + m_contactOffset;

	Vector3 from = m_transform->Up();

	if (m_alignement.m_isRotating)
		AlignToGravity();
	else if (from != m_alignement.m_to)
	{
		m_alignement.m_isRotating = true;

		m_alignement.m_startRotation = m_transform->GetRotation();

		Quaternion rotationToUp = Quaternion::FromToRotation(from, Vector3::Up);
		Vector3 rotatedForward = Quaternion::RotateVector3(rotationToUp, m_transform->Forward());
		Quaternion verticalRotation = Quaternion::FromToRotation(-Vector3::Forward, rotatedForward);
		m_alignement.m_goalRotation = verticalRotation * Quaternion::FromToRotation(Vector3::Up, m_alignement.m_to);

		AlignToGravity();
	}

	PhysicSystem::RaycastHit hit = m_physicActor->Raycast(m_transform->GetGlobalPosition(), m_raycastDir, raycastDist);

	physx::PxVec3 pxVel = m_dynamicActor->getLinearVelocity();
	CCMaths::Vector3 vel = { pxVel.x, pxVel.y, pxVel.z };

	physx::PxVec3 pxRVel = m_dynamicActor->getAngularVelocity();
	CCMaths::Vector3 rVel = { pxRVel.x, pxRVel.y, pxRVel.z };

	if (hit.actor && !hit.collider->GetTrigger())
	{
		m_isGrounded = true;

		float deltaDistance = hit.distance - raycastDist;
		float downVel = CCMaths::Vector3::Dot(m_raycastDir, vel);

		float force = deltaDistance * m_springStrength - downVel * m_springDampling;

		m_physicActor->AddForce(m_raycastDir * force, PhysicSystem::EForceMode::eFORCE);
	}
	else
	{
		m_isGrounded = false;
	}

	float moveSpeed = m_isRunning ? m_runningSpeed : m_walkingSpeed;
	CCMaths::Vector3 move = m_transform->Forward() * m_forwardMove + m_transform->Right() * m_sideMove;
	CCMaths::Vector3 goalVelocity = move * moveSpeed;
	CCMaths::Vector3 neededAcceleration = CCMaths::Vector3::ClampLength((goalVelocity - vel) / TimeManager::GetFixedDeltaTime(), -150.f, 150.f);
	CCMaths::Vector3 neededForce = CCMaths::Vector3::Multiply(neededAcceleration * m_dynamicActor->getMass(), { 1, 0, 1 });

	m_physicActor->AddForce(neededForce, PhysicSystem::EForceMode::eFORCE);
}

void CharacterController::AlignToGravity()
{
	m_alignement.m_lerpPercent += 0.01f;
	
	if (m_alignement.m_lerpPercent >= 1.0f)
	{
		m_alignement.m_lerpPercent = 0.0f;
		m_alignement.m_isRotating = false;

		m_transform->SetRotation(m_alignement.m_goalRotation);

		return;
	}

	Quaternion newRot = Quaternion::SLerp(m_alignement.m_startRotation, m_alignement.m_goalRotation, m_alignement.m_lerpPercent);

	m_transform->SetRotation(newRot);
}

void CharacterController::InvalidateTransform()
{
	m_transform = nullptr;
}
