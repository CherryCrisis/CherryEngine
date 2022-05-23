#pragma once

#include "behaviour.hpp"

#include "maths.hpp"

#include "portal_component.hpp"

#include "property.hpp"

class MeshRenderer;
class Transform;

class CCENGINE_API PortalTeleporterComponent : public Behaviour
{
protected:
	void PopulateMetadatas() override;

public:
	CCMaths::Vector3 m_previousOffsetFromPortal = Vector3::Zero;

	Transform* m_transform = nullptr;
	MeshRenderer* m_meshRenderer = nullptr;

	Transform* m_cloneTransform = nullptr;
	MeshRenderer* m_cloneMeshRenderer = nullptr;

	PortalTeleporterComponent();
	PortalTeleporterComponent(CCUUID& id);
	~PortalTeleporterComponent();

	void Initialize();
	void BindToSignals() override;

	void EnterPortal(const PortalComponent* linkedPortal, const CCMaths::Vector3& newPos, const CCMaths::Vector3& newRot, const CCMaths::Vector3& newScale);
	void UpdateEntityClone(const CCMaths::Vector3& newPos, const CCMaths::Vector3& newRot, const CCMaths::Vector3& newScale);
	void ExitPortal();

	void Teleport(PortalComponent* destPortal, const CCMaths::Vector3& newPos, const CCMaths::Vector3& newRot, const CCMaths::Vector3& newScale);

	//CCProperty::CopyProperty<PortalComponent, Behaviour*> m_LinkedPortalProp{ this, &PortalComponent::SetLinkedPortal, &PortalComponent::GetLinkedPortal };
};
