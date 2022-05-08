#pragma once

#include "behaviour.hpp"

#include "maths.hpp"

#include "portal.hpp"

#include "property.hpp"

class Transform;

class CCENGINE_API PortalComponent : public Behaviour
{
protected:
	void PopulateMetadatas() override;

public:
	Portal	   m_portal;
	Transform* m_transform = nullptr;

	PortalComponent* m_linkedPortal = nullptr;

	PortalComponent();
	PortalComponent(CCUUID& id);
	~PortalComponent();

	void Initialize();
	void BindToSignals() override;

	void ChangePosition(const CCMaths::Vector3& position);
	void ChangeRotation(const CCMaths::Vector3& rotation);
	void ChangeScale(const CCMaths::Vector3& scale);

	void SetLinkedPortal(Object* linkedObject)
	{
		auto linkedPortal = dynamic_cast<PortalComponent*>(linkedObject);

		if (!linkedPortal)
			return;
		m_linkedPortal = linkedPortal;

		if (linkedPortal)
			m_portal.m_linkedPortal = &linkedPortal->m_portal;
	}

	Object* GetLinkedPortal()
	{
		return m_linkedPortal;
	}

	CCProperty::CopyProperty<PortalComponent, Object*> m_LinkedPortalProp { this, &PortalComponent::SetLinkedPortal, &PortalComponent::GetLinkedPortal };
};