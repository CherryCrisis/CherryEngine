#pragma once

#include "maths.hpp"
#include "renderer.hpp"

struct GPULight { };


struct Light
{
    alignas(16) bool m_enabled = true;
    alignas(16) bool m_isPoint = true;
    alignas(16) CCMaths::Vector3 m_position = CCMaths::Vector3::Zero;
    alignas(16) CCMaths::Vector3 m_ambient = CCMaths::Vector3::One;
    alignas(16) CCMaths::Vector3 m_diffuse = CCMaths::Vector3::One;
    alignas(16) CCMaths::Vector3 m_specular = CCMaths::Vector3::One;
    alignas(16) CCMaths::Vector3 m_attenuation = CCMaths::Vector3(1.f, 0.f, 0.f);

    alignas(16) CCMaths::Matrix4 m_lightSpace = CCMaths::Matrix4::Identity;

    alignas(16) CCMaths::Matrix4 m_lookAtMatrix = CCMaths::Matrix4::Identity;

    std::unique_ptr<GPULight> m_gpuLight = nullptr;
};