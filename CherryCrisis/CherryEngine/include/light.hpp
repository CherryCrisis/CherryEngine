#pragma once

#include "maths.hpp"

struct Light
{
    alignas(16) int m_enabled;
    alignas(16) CCMaths::Vector4 m_position;
    alignas(16) CCMaths::Vector3 m_ambient = CCMaths::Vector3::Zero;
    alignas(16) CCMaths::Vector3 m_diffuse = CCMaths::Vector3::One;
    alignas(16) CCMaths::Vector3 m_specular = CCMaths::Vector3::Zero;
    alignas(16) CCMaths::Vector3 m_attenuation = CCMaths::Vector3(1.f, 0.f, 0.f);
};