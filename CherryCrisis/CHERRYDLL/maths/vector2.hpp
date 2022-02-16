#pragma once

namespace CCMaths
{
	struct Vector2
	{
		union
		{
			struct { float x, y; };
			float data[2];
		};
		
		Vector2()
			:x(0.f), y(0.f)
		{

		}

		Vector2(const float value)
			:x(value), y(value)
		{

		}

		constexpr Vector2(const float x, const float y)
			: x(x), y(y)
		{

		}

		constexpr static Vector2 XAxis() { return Vector2(1.f, 0.f); };
		constexpr static Vector2 YAxis() { return Vector2(0.f, 1.f); };

		inline Vector2 operator+(const Vector2& rhs) const;
		inline Vector2 operator+(const float rhs) const;
		inline Vector2& operator+=(const Vector2& rhs);
		inline Vector2& operator+=(const float rhs);

		inline Vector2 operator-() const;
		inline Vector2 operator-(const Vector2& rhs) const;
		inline Vector2 operator-(const float rhs) const;
		inline Vector2& operator-=(const Vector2& rhs);
		inline Vector2& operator-=(const float rhs);

		inline float operator*(const Vector2& rhs) const;
		inline Vector2 operator*(const float rhs) const;
		inline Vector2& operator*=(const float rhs);

		inline Vector2 operator/(const float rhs) const;
		inline Vector2& operator/=(const float rhs);

		inline bool operator==(const Vector2& rhs) const;
		inline bool operator!=(const Vector2& rhs) const;

		inline float Dot(const Vector2& rhs) const;
		inline static float Dot(const Vector2& lhs, const Vector2& rhs);

		inline float SquareLength() const;
		inline float Length() const;

		inline Vector2& Normalize();
		inline Vector2& Normalize(Vector2& lhs);

		inline Vector2 Normalized() const;
		inline static Vector2 Normalized(const Vector2& lhs);
	};
}

#include "vector2.inl"