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

	bool test = false;

	PortalComponent* m_linkedPortal = nullptr;

	PortalComponent();
	PortalComponent(CCUUID& id);
	~PortalComponent();

	void Initialize();
	void BindToSignals() override;

	void ChangePosition(const CCMaths::Vector3& position);
	void ChangeRotation(const CCMaths::Vector3& rotation);
	void ChangeScale(const CCMaths::Vector3& scale);
	void OnCellAdded(Cell* newCell);
	void OnCellRemoved(Cell* newCell);

	void InvalidateLinkedPortal();

	void SetLinkedPortal(Object* linkedObject);
	Object* GetLinkedPortal();

	CCProperty::CopyProperty<PortalComponent, Object*> m_LinkedPortalProp { this, &PortalComponent::SetLinkedPortal, &PortalComponent::GetLinkedPortal };
};