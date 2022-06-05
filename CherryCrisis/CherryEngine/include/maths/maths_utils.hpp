#pragma once

#include <cmath>

//! \brief #include <maths.hpp>
namespace CCMaths
{
    constexpr static float TAU = 6.283185307f;
    constexpr static float PI = 3.141592653f;
    constexpr static float DEG2RAD = CCMaths::PI / 180.f;
    constexpr static float RAD2DEG = 180.f / CCMaths::PI;
    constexpr static float epsilon = 0.00001f;

    inline float ToRadians(float angle);
    inline float ToDegrees(float angle);
    inline float Cos(float rad);
    inline float Sin(float rad);
    inline float Tan(float rad);
    inline float Atan(float value);
    inline float Abs(float value);

    inline float Sqrt(float value);

    inline float InverseSqrt(float value);

    constexpr bool BoolPattern(int index, unsigned int pattern = 2u);

    constexpr int BoolSign(bool flag);

    template<typename T>
    constexpr T Remap(const T& value, const T& oldMin, const T& oldMax, const T& newMin, const T& newMax);

    inline float Round(const double& value);

    template <typename T>
    int Sign(const T& value);

    template<typename T>
    T Min(const T& lhs, const T& rhs);

    template<typename T>
    T Max(const T& lhs, const T& rhs);

    template<typename T>
    T Clamp(const T& X, const T& minValue, const T& maxValue);

    template<typename T, typename S>
    T Lerp(const T& lhs, const T& rhs, const S& factor);
}

#include "maths_utils.inl"