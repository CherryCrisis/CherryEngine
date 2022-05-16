#include "pch.hpp"

#include "sphere_collider.hpp"

#include <PxPhysicsAPI.h>

#include "resource_manager.hpp"
#include "physic_manager.hpp"

#include "collider_renderpass.hpp"
#include "camera_component.hpp"
#include "transform.hpp"
#include "mesh.hpp"

SphereCollider::SphereCollider()
{
	PopulateMetadatas();

	m_sphereCollider = ResourceManager::GetInstance()->AddResource<Mesh>("CC_NormalizedSphere", true, EMeshShape::SPHERE, 0.5f, 8.f, 16.f);

	Camera* cam = CameraComponent::m_editorCamera;
	if (!cam)
		return;

	SubscribeToPipeline(cam->m_pipeline.get());
}

SphereCollider::SphereCollider(CCUUID& id) : Collider(id)
{
	PopulateMetadatas();

	m_sphereCollider = ResourceManager::GetInstance()->AddResource<Mesh>("CC_NormalizedSphere", true, EMeshShape::SPHERE, 0.5f, 8.f, 16.f);

	Camera* cam = CameraComponent::m_editorCamera;
	if (!cam)
		return;

	SubscribeToPipeline(cam->m_pipeline.get());
}

SphereCollider::~SphereCollider()
{
	Unregister();

	if (m_transform)
	{
		m_transform->m_onScaleChange.Unbind(&SphereCollider::SetEntityScale, this);
		m_transform->m_OnDestroy.Unbind(&SphereCollider::InvalidateTransform, this);
	}
}

void SphereCollider::BindToSignals()
{
	PhysicSystem::PhysicManager* physicManager = PhysicSystem::PhysicManager::GetInstance();

	physicManager->Register(this);
	m_isRegistered = true;

	m_transform = m_physicActor->m_owner->GetOrAddBehaviour<Transform>();

	m_transform->m_onScaleChange.Bind(&SphereCollider::SetEntityScale, this);
	m_transform->m_OnDestroy.Bind(&SphereCollider::InvalidateTransform, this);

	SetEntityScale(m_transform->GetScale());
}

void SphereCollider::InvalidateTransform()
{
	m_transform = nullptr;
}

void SphereCollider::Unregister()
{
	if (m_isRegistered)
	{
		PhysicSystem::PhysicManager* physicManager = PhysicSystem::PhysicManager::GetInstance();

		physicManager->Unregister(this);
		m_isRegistered = false;
	}
}

void SphereCollider::PopulateMetadatas()
{
	Behaviour::PopulateMetadatas();

	m_metadatas.SetProperty("Enabled", &isEnabled);
	m_metadatas.SetProperty("Is Trigger", &isTrigger);
	m_metadatas.SetProperty("Local Position", &localPosition);
	m_metadatas.SetProperty("Scale", &editableScale);
	m_metadatas.SetProperty("Contact Offset", &contactOffset);
}

void SphereCollider::SetEntityScale(const CCMaths::Vector3& s)
{
	CCMaths::Vector3 scale = m_transform->GetGlobalScale();
	m_entityScale = (scale.x + scale.y + scale.z) / 3.f;

	m_totalScale = m_baseEntityScale * m_editableScale * m_entityScale;
}

void SphereCollider::SetPxShape()
{
	m_pxShape = m_physicActor->CreateShape(physx::PxSphereGeometry(m_totalScale));
	SetPxLocalPos();

	m_pxShape->userData = this;

	SetPxData();
}

void SphereCollider::ClearPxShape()
{
	if (m_pxShape)
	{
		m_physicActor->RemoveShape(m_pxShape);
		m_pxShape = nullptr;
	}
}

void SphereCollider::ResetPxShape()
{
	if (!m_pxShape)
		return;

	ClearPxShape();
	SetPxShape();
}

void SphereCollider::SetPxData()
{
	if (m_pxShape)
	{
		m_pxShape->setContactOffset(m_contactOffset);

		if (!m_isEnabled)
		{
			m_pxShape->setFlag(physx::PxShapeFlag::Enum::eSIMULATION_SHAPE, false);
			m_pxShape->setFlag(physx::PxShapeFlag::Enum::eTRIGGER_SHAPE, false);
			m_pxShape->setFlag(physx::PxShapeFlag::Enum::eSCENE_QUERY_SHAPE, false);
		}
		else if (m_isTrigger)
		{
			m_pxShape->setFlag(physx::PxShapeFlag::Enum::eSIMULATION_SHAPE, false);
			m_pxShape->setFlag(physx::PxShapeFlag::Enum::eTRIGGER_SHAPE, true);
			m_pxShape->setFlag(physx::PxShapeFlag::Enum::eSCENE_QUERY_SHAPE, true);
		}
		else
		{
			m_pxShape->setFlag(physx::PxShapeFlag::Enum::eTRIGGER_SHAPE, false);
			m_pxShape->setFlag(physx::PxShapeFlag::Enum::eSIMULATION_SHAPE, true);
			m_pxShape->setFlag(physx::PxShapeFlag::Enum::eSCENE_QUERY_SHAPE, true);
		}
	}
}

void SphereCollider::SubscribeToPipeline(ARenderingPipeline* pipeline)
{
	if (!m_sphereCollider)
		return;

	pipeline->SubscribeToPipeline<ColliderRenderPass>(dynamic_cast<Collider*>(this));
}

void SphereCollider::UnsubscribeToPipeline(ARenderingPipeline* pipeline)
{
	pipeline->UnsubscribeToPipeline<ColliderRenderPass>(dynamic_cast<Collider*>(this));
}

void SphereCollider::SetScale(const float& scale)
{
	m_editableScale = scale;

	m_totalScale = m_baseEntityScale * m_editableScale * m_entityScale;

	ResetPxShape();
}

CCMaths::Matrix4 SphereCollider::GetTranformMatrix()
{
	return m_transform->GetWorldMatrix().NormalizedScale() * CCMaths::Matrix4::Translate(m_localPosition) * CCMaths::Matrix4::Scale(m_totalScale);
}

Mesh* SphereCollider::GetMesh()
{
	return m_sphereCollider.get();
}