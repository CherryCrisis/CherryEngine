#pragma once

#include "vector3.hpp"

namespace CCMaths
{
	struct Vector4
	{
		union
		{
			struct { float x, y, z, w; };
			struct { float r, g, b, a; };
			float data[4];

			Vector3 xyz;
			Vector3 rgb;
		};

		Vector4()
			: x(0.f), y(0.f), z(0.f), w(0.f) {}

		Vector4(const float value)
			: x(value), y(value), z(value), w(value) {}

		constexpr Vector4(float x, float y, float z, float w)
			: x(x), y(y), z(z), w(w) {}

		Vector4(const Vector3& xyz, float w)
			: xyz(xyz)
		{
			this->w = w;
		}

		constexpr static Vector4 XAxis() { return Vector4(1.f, 0.f, 0.f, 0.f); };
		constexpr static Vector4 YAxis() { return Vector4(0.f, 1.f, 0.f, 0.f); };
		constexpr static Vector4 ZAxis() { return Vector4(0.f, 0.f, 1.f, 0.f); };
		constexpr static Vector4 WAxis() { return Vector4(0.f, 0.f, 0.f, 1.f); };

		inline Vector4 operator+(const Vector4& rhs) const;
		inline Vector4 operator+(const float rhs) const;
		inline Vector4& operator+=(const Vector4& rhs);
		inline Vector4& operator+=(const float rhs);

		inline Vector4 operator-() const;
		inline Vector4 operator-(const Vector4& rhs) const;
		inline Vector4 operator-(const float rhs) const;
		inline Vector4& operator-=(const Vector4& rhs);
		inline Vector4& operator-=(const float rhs);

		inline float operator*(const Vector4& rhs) const;
		inline Vector4 operator*(const float rhs) const;
		inline Vector4& operator*=(const float rhs);

		inline Vector4 operator/(const float rhs) const;
		inline Vector4& operator/=(const float rhs);

		inline bool operator==(const Vector4& rhs) const;
		inline bool operator!=(const Vector4& rhs) const;

		inline float Dot(const Vector4& rhs) const;
		inline static float Dot(const Vector4& lhs, const Vector4& rhs);

		inline float SquareLength() const;
		inline float Length() const;

		inline Vector4& Normalize();
		inline static Vector4& Normalize(Vector4& outVector);

		inline Vector4 Normalized() const;
		inline static Vector4 Normalized(const Vector4& inVector);
	};
}

#include "vector4.inl"