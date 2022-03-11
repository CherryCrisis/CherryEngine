#pragma once

#include "behaviour.hpp"

#include "light.hpp"

class LightComponent : public Behaviour
{
public:
	LightComponent(Entity& owner);
	~LightComponent();

	Light m_light;
};