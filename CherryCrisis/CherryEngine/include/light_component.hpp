#pragma once

#include "Component.h"

#include "light.hpp"

class LightComponent : public Behaviour
{
public:
	LightComponent();
	~LightComponent();

	Light m_light;
};