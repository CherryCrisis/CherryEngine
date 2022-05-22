#pragma once

#include "behaviour.hpp"

#include "maths.hpp"

#include "portal_component.hpp"

#include "property.hpp"

class Transform;

class CCENGINE_API PortalTeleporterComponent : public Behaviour
{
protected:
	void PopulateMetadatas() override;

public:
	CCMaths::Vector3 m_previousOffsetFromPortal = Vector3::Zero;
	Transform* m_transform = nullptr;

	PortalTeleporterComponent();
	PortalTeleporterComponent(CCUUID& id);
	~PortalTeleporterComponent();

	void Initialize();
	void BindToSignals() override;

	void EnterPortal();
	void ExitPortal();

	void Teleport(PortalComponent* destPortal, const CCMaths::Vector3& newPos);

	//CCProperty::CopyProperty<PortalComponent, Behaviour*> m_LinkedPortalProp{ this, &PortalComponent::SetLinkedPortal, &PortalComponent::GetLinkedPortal };
};
