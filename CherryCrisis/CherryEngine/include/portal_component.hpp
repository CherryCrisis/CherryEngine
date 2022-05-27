#pragma once

#include "behaviour.hpp"
#include "maths.hpp"
#include "portal.hpp"
#include "property.hpp"

class Transform;
class PortalTeleporterComponent;


class CCENGINE_API PortalComponent : public Behaviour
{
protected:
	void PopulateMetadatas() override;

public:
	Portal	m_portal;
	Transform* m_transform = nullptr;

	PortalComponent* m_linkedPortal = nullptr;

	std::set<PortalTeleporterComponent*> m_portalTeleporters;

	PortalComponent();
	PortalComponent(CCUUID& id);
	~PortalComponent();

	void Initialize();
	void BindToSignals() override;
	void LateUpdate() override;

	void UpdatePortalMatrices(Transform* tranform);
	void OnCellAdded(Cell* newCell);
	void OnCellRemoved(Cell* newCell);

	void InvalidateLinkedPortal();

	void SetLinkedPortal(Behaviour* linkedObject);
	Behaviour* GetLinkedPortal();

	void OnTriggerEnter(Entity* other);
	void OnTriggerExit(Entity* other);

	void OnEntityEnter(PortalTeleporterComponent* portalTeleporter);
	void UpdateSliceParamaters(PortalTeleporterComponent* portalTeleporter);

	CCProperty::CopyProperty<PortalComponent, Behaviour*> m_LinkedPortalProp { this, &PortalComponent::SetLinkedPortal, &PortalComponent::GetLinkedPortal };
};
