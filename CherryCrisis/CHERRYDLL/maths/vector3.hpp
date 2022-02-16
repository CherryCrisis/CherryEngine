#pragma once

#include "vector2.hpp"

namespace CCMaths
{
	struct Vector3
	{
		union
		{
			struct { float x, y, z; };
			struct { float r, g, b; };
			float data[3];
		
			Vector2 xy;
		};

		Vector3()
			:x(0.f), y(0.f), z(0.f)
		{

		}

		Vector3(const float value)
			:x(value), y(value), z(value)
		{

		}

		constexpr Vector3(const float x, const float y, const float z)
			: x(x), y(y), z(z)
		{

		}

		Vector3(const Vector2& xy, float z)
			: xy(xy)
		{
			this->z = z;
		}

		constexpr static Vector3 Forward() { return Vector3::ZAxis(); };
		constexpr static Vector3 Up() { return Vector3::YAxis(); };
		constexpr static Vector3 Right() { return Vector3::XAxis(); };

		constexpr static Vector3 XAxis() { return Vector3(1.f, 0.f, 0.f); };
		constexpr static Vector3 YAxis() { return Vector3(0.f, 1.f, 0.f); };
		constexpr static Vector3 ZAxis() { return Vector3(0.f, 0.f, 1.f); };

		inline Vector3 operator+(const Vector3& rhs) const;
		inline Vector3 operator+(const float rhs) const;
		inline Vector3& operator+=(const Vector3& rhs);
		inline Vector3& operator+=(const float rhs);

		inline Vector3 operator-() const;
		inline Vector3 operator-(const Vector3& rhs) const;
		inline Vector3 operator-(const float rhs) const;
		inline Vector3& operator-=(const Vector3& rhs);
		inline Vector3& operator-=(const float rhs);

		inline float operator*(const Vector3& rhs) const;
		inline Vector3 operator*(const float rhs) const;
		inline Vector3& operator*=(const float rhs);

		inline Vector3 operator^(const Vector3& rhs) const;

		inline Vector3 operator/(const float rhs) const;
		inline Vector3& operator/=(const float rhs);

		inline bool operator==(const Vector3& rhs) const;
		inline bool operator!=(const Vector3& rhs) const;

		inline float Dot(const Vector3& rhs) const;
		inline static float Dot(const Vector3& lhs, const Vector3& rhs);

		inline Vector3 Cross(const Vector3& rhs) const;
		inline static Vector3 Cross(const Vector3& lhs, const Vector3& rhs);

		inline float SquareLength() const;
		inline float Length() const;

		inline Vector3& Normalize();
		inline Vector3& Normalize(Vector3& lhs);

		inline Vector3 Normalized() const;
		inline static Vector3 Normalized(const Vector3& lhs);
	};
}

#include "vector3.inl"