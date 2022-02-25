#pragma once

#include "component.hpp"

#include "light.hpp"

class LightComponent : public Behaviour
{
public:
	LightComponent();
	~LightComponent();

	Light m_light;
};