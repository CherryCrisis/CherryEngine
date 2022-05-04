#pragma once

#include "behaviour.hpp"

#include "maths.hpp"

#include "portal.hpp"

class Transform;

class CCENGINE_API PortalComponent : public Behaviour
{
protected:
	void PopulateMetadatas() override;

public:
	Portal	   m_portal;
	Transform* m_transform = nullptr;

	PortalComponent();
	PortalComponent(CCUUID& id);
	~PortalComponent();

	void Initialize();
	void BindToSignals() override;

	void ChangePosition(const CCMaths::Vector3& position);
	void ChangeRotation(const CCMaths::Vector3& rotation);
	void ChangeScale(const CCMaths::Vector3& scale);
};