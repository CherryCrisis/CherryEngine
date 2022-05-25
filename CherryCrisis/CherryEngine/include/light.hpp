#pragma once

#include "maths.hpp"
#include "renderer.hpp"

#include <memory>

#include "event.hpp"

struct GPULight { };

enum class ELightType : unsigned int
{
    OFF,
    POINT,
    DIRECTIONAL,
};

struct Light
{
    alignas(16) CCMaths::Vector3 m_position = CCMaths::Vector3::Zero;
    alignas(16) CCMaths::Vector3 m_direction = -CCMaths::Vector3::Up;
    alignas(16) CCMaths::Vector3 m_ambient = CCMaths::Vector3::One;
    alignas(16) CCMaths::Vector3 m_diffuse = CCMaths::Vector3::One;
    alignas(16) CCMaths::Vector3 m_specular = CCMaths::Vector3::One;
    alignas(16) CCMaths::Vector3 m_attenuation = CCMaths::Vector3(1.f, 0.f, 0.f);

    alignas(16) CCMaths::Matrix4 m_lightSpace = CCMaths::Matrix4::Identity;

    alignas(4) ELightType m_type = ELightType::POINT;
    alignas(4) float m_cutoff = cosf(CCMaths::PI);
    alignas(4) float m_outerCutoff = cosf(CCMaths::PI);

    alignas(16) CCMaths::Matrix4 m_lookAtMatrix = CCMaths::Matrix4::Identity;

    std::unique_ptr<GPULight> m_gpuLight = nullptr;

    Event<unsigned int, size_t, void*> m_OnParamsChanged;
};