#pragma once

#include <cmath>

#include "maths/quaternion.hpp"

namespace CCMaths
{
    constexpr static float TAU = 6.283185307f;
    constexpr static float PI = 3.141592653f;
    constexpr static float DEG2RAD = CCMaths::PI / 180.f;
    constexpr static float RAD2DEG = 180.f / CCMaths::PI;

    inline float ToRadians(float angle) { return angle * DEG2RAD; }
    inline float ToDegrees(float angle) { return angle * RAD2DEG; }
    inline float Cos(float rad) { return std::cos(rad); }
    inline float Sin(float rad) { return std::sin(rad); }
    inline float Tan(float rad) { return std::tan(rad); }
    inline float Atan(float value) { return std::atan(value); }

    inline float Sqrt(float value) { return std::sqrt(value); }

    constexpr bool BoolPattern(int index, unsigned int pattern = 2u) { return index % (2 * pattern) < pattern; }

    constexpr int BoolSign(bool flag) { return 1 - 2 * flag; }

    template<typename T>
    T Remap(const T& value, const T& oldMin, const T& oldMax, const T& newMin, const T& newMax)
    {
        return (value - oldMin) * (newMax - newMin) / (oldMax - oldMin) + newMin;
    }

    template <typename T>
    int Sign(const T& value) { return (T(0) < value) - (value < T(0)); }

    template<typename T>
    T Min(const T& lhs, const T& rhs) { return lhs < rhs ? lhs : rhs; }

    template<typename T>
    T Max(const T& lhs, const T& rhs) { return lhs > rhs ? lhs : rhs; }

    template<typename T>
    T Clamp(const T& X, const T& minValue, const T& maxValue) { return Min(Max(X, minValue), maxValue); }

    template<typename T, typename S>
    T Lerp(const T& lhs, const T& rhs, const S& factor) { return (1.f - factor) * lhs + factor * rhs; }
}