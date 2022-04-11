#pragma once

#include "behaviour.hpp"

#include "light.hpp"

class Transform;

class LightComponent : public Behaviour
{
protected:
	void PopulateMetadatas() override;

public:
	Light		m_light;
	Transform*	m_transform = nullptr;

	LightComponent();
	~LightComponent();

	void Initialize();
	void BindToSignals() override;

	void ChangePosition(const CCMaths::Vector3& position);
	void ChangeRotation(const CCMaths::Vector3& rotation);
};