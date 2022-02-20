#pragma once

#include "vector4.hpp"

namespace CCMaths
{
	struct Matrix4
	{
		union
		{
			float data[16];
			Vector4 row[4];
		};

		inline static const Matrix4 Identity();

		inline Matrix4 operator*(const float rhs);
		inline Vector4 operator*(const Vector4& rhs);
		inline Matrix4 operator*(const Matrix4& rhs);

		inline Matrix4& operator*=(const float rhs);
		inline Matrix4& operator*=(const Matrix4& rhs);

		inline Matrix4 Transpose() const;
		inline static Matrix4 Transpose(const Matrix4& lhs);

		inline Matrix4 Invert() const;
		inline static Matrix4 Invert(const Matrix4& lhs);

		inline static Matrix4 Translate(const Vector3& rhs);
		inline static Matrix4 Scale(const float rhs);
		inline static Matrix4 Scale(const Vector3& rhs);

		inline static Matrix4 Rotate(const Vector3& eulerAngles);

		inline static Matrix4 RotateX(const float rad);
		inline static Matrix4 RotateY(const float rad);
		inline static Matrix4 RotateZ(const float rad);
		inline static Matrix4 RotateX(const float cos, const float sin);
		inline static Matrix4 RotateY(const float cos, const float sin);
		inline static Matrix4 RotateZ(const float cos, const float sin);

		inline static Matrix4 Frustum(const float Left, const float Right, const float Bottom, const float Top, const float Near, const float Far);

		inline static Matrix4 Perspective(const float FovY, const float Aspect, const float Near, const float Far);
	};
}

#include "matrix4.inl"