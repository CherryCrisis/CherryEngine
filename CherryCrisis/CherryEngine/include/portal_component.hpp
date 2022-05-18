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

	void UpdatePortalMatrices(const CCMaths::Vector3& v);
	void OnCellAdded(Cell* newCell);
	void OnCellRemoved(Cell* newCell);

	void InvalidateLinkedPortal();

	void SetLinkedPortal(Behaviour* linkedObject);
	Behaviour* GetLinkedPortal();

	CCProperty::CopyProperty<PortalComponent, Behaviour*> m_LinkedPortalProp { this, &PortalComponent::SetLinkedPortal, &PortalComponent::GetLinkedPortal };
};