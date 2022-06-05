#include <cmath>

namespace CCMaths
{
    inline float ToRadians(float angle) { return angle * DEG2RAD; }
    inline float ToDegrees(float angle) { return angle * RAD2DEG; }
    inline float Cos(float rad) { return std::cos(rad); }
    inline float Sin(float rad) { return std::sin(rad); }
    inline float Tan(float rad) { return std::tan(rad); }
    inline float Atan(float value) { return std::atan(value); }
    inline float Abs(float value) { return std::abs(value); }

    inline float Sqrt(float value) { return std::sqrt(value); }

    // From Quake III
    inline float InverseSqrt(float value)
    {
        long i;
        float x2, y;
        const float threehalfs = 1.5f;

        x2 = value * 0.5f;
        y = value;
        i = *(long*)&y;
        i = 0x5f3759df - (i >> 1);
        y = *(float*)&i;

        //Newton Iterations
        for (int j = 0; j < 10; j++)
            y = y * (threehalfs - (x2 * y * y));    

        return y;
    }

    constexpr bool BoolPattern(int index, unsigned int pattern) { return index % (2 * pattern) < pattern; }

    constexpr int BoolSign(bool flag) { return 1 - 2 * !flag; }

    template<typename T>
    constexpr T Remap(const T& value, const T& oldMin, const T& oldMax, const T& newMin, const T& newMax)
    {
        return (value - oldMin) * (newMax - newMin) / (oldMax - oldMin) + newMin;
    }

    inline float Round(const double& value) { return (float)floor(value + 0.5); }

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
