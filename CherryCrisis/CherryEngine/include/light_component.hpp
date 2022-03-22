#pragma once

#include "behaviour.hpp"

#include "light.hpp"

class LightComponent : public Behaviour
{
protected:
	void PopulateMetadatas() override;

public:
	LightComponent(Entity& owner);
	LightComponent() { PopulateMetadatas(); };
	~LightComponent();

	Light m_light;
};