#pragma once

#include <cmath>

#include "maths/quaternion.hpp"

namespace CCMaths
{
    constexpr static float tau = 6.283185307f;
    constexpr static float pi = 3.141592653f;

    //! Function
    inline float ToRadians(float angle) { return angle * CCMaths::pi / 180.f; }
    inline float ToDegrees(float angle) { return angle * 180.f / CCMaths::pi; }
    inline float Cos(float rad) { return std::cos(rad); }
    inline float Sin(float rad) { return std::sin(rad); }
    inline float Tan(float rad) { return std::tan(rad); }
    inline float Atan(float value) { return std::atan(value); }

    inline float Sqrt(float value) { return std::sqrt(value); }

    inline float Sign(float value) { return value < 0.f ? -1.f : 1.f; }
    inline int Sign(int value) { return value < 0 ? -1 : 1; }

    template<typename T>
    inline T Min(T lhs, T rhs) { return lhs < rhs ? lhs : rhs; }

    template<typename T>
    inline T Max(T lhs, T rhs) { return lhs > rhs ? lhs : rhs; }

    template<typename T>
    inline T Clamp(T X, T minValue, T maxValue) { return Min(Max(X, minValue), maxValue); }

    template<typename T>
    inline T Lerp(T lhs, T rhs, float A) { return (1.f - A) * lhs + A * rhs; }
}