#include <cmath>

namespace CCMaths
{
	inline Matrix4 Matrix4::operator*(const float rhs) const
	{
		Matrix4 out = *this;
		
		for (int i = 0; i < 16; ++i)
		{
			out.data[i] *= rhs;
		}

		return out;
	}

	inline Vector4 Matrix4::operator*(const Vector4& rhs) const
	{
		Vector4 out;

		for (int i = 0; i < 4; ++i)
		{
			out.data[i] = Vector4::Dot(this->row[i], rhs);
		}

		return out;
	}

	inline Matrix4 Matrix4::operator*(const Matrix4& rhs) const
	{
		Matrix4 rhsT = Transpose(rhs);
		Matrix4 out = {};

		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				out.row[i].data[j] = Vector4::Dot(this->row[i], rhsT.row[j]);
			}
		}

		return out;
	}

	inline Matrix4& Matrix4::operator*=(const float rhs)
	{
		*this = *this * rhs;
		return *this;
	}

	inline Matrix4& Matrix4::operator*=(const Matrix4& rhs)
	{
		*this = *this * rhs;
		return *this;
	}

	inline Matrix4 Matrix4::Transpose() const
	{
		return Matrix4::Transpose(*this);
	}

	inline Matrix4 Matrix4::Transpose(const Matrix4& lhs)
	{
		return
		{
			lhs.data[0], lhs.data[4], lhs.data[8],  lhs.data[12],
			lhs.data[1], lhs.data[5], lhs.data[9],  lhs.data[13],
			lhs.data[2], lhs.data[6], lhs.data[10], lhs.data[14],
			lhs.data[3], lhs.data[7], lhs.data[11], lhs.data[15],
		};
	}

	inline Matrix4 Matrix4::Inverse() const
	{
		return Matrix4::Inverse(*this);
	}

	inline Matrix4 Matrix4::Inverse(const Matrix4& lhs)
	{
		Matrix4 result = { 0 };

		// Cache the matrix values (speed optimization)
		float a00 = lhs.data[0], a01 = lhs.data[1], a02 = lhs.data[2], a03 = lhs.data[3];
		float a10 = lhs.data[4], a11 = lhs.data[5], a12 = lhs.data[6], a13 = lhs.data[7];
		float a20 = lhs.data[8], a21 = lhs.data[9], a22 = lhs.data[10], a23 = lhs.data[11];
		float a30 = lhs.data[12], a31 = lhs.data[13], a32 = lhs.data[14], a33 = lhs.data[15];

		float b00 = a00 * a11 - a01 * a10;
		float b01 = a00 * a12 - a02 * a10;
		float b02 = a00 * a13 - a03 * a10;
		float b03 = a01 * a12 - a02 * a11;
		float b04 = a01 * a13 - a03 * a11;
		float b05 = a02 * a13 - a03 * a12;
		float b06 = a20 * a31 - a21 * a30;
		float b07 = a20 * a32 - a22 * a30;
		float b08 = a20 * a33 - a23 * a30;
		float b09 = a21 * a32 - a22 * a31;
		float b10 = a21 * a33 - a23 * a31;
		float b11 = a22 * a33 - a23 * a32;

		float det = (b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06);

		result.data[0] = (a11 * b11 - a12 * b10 + a13 * b09);
		result.data[1] = (-a01 * b11 + a02 * b10 - a03 * b09);
		result.data[2] = (a31 * b05 - a32 * b04 + a33 * b03);
		result.data[3] = (-a21 * b05 + a22 * b04 - a23 * b03);
		result.data[4] = (-a10 * b11 + a12 * b08 - a13 * b07);
		result.data[5] = (a00 * b11 - a02 * b08 + a03 * b07);
		result.data[6] = (-a30 * b05 + a32 * b02 - a33 * b01);
		result.data[7] = (a20 * b05 - a22 * b02 + a23 * b01);
		result.data[8] = (a10 * b10 - a11 * b08 + a13 * b06);
		result.data[9] = (-a00 * b10 + a01 * b08 - a03 * b06);
		result.data[10] = (a30 * b04 - a31 * b02 + a33 * b00);
		result.data[11] = (-a20 * b04 + a21 * b02 - a23 * b00);
		result.data[12] = (-a10 * b09 + a11 * b07 - a12 * b06);
		result.data[13] = (a00 * b09 - a01 * b07 + a02 * b06);
		result.data[14] = (-a30 * b03 + a31 * b01 - a32 * b00);
		result.data[15] = (a20 * b03 - a21 * b01 + a22 * b00);

		return result * (1.0f / det);
	}

	inline Matrix4 Matrix4::Translate(const Vector3& in)
	{
		return
		{
			1.f,  0.f,  0.f,  0.f,
			0.f,  1.f,  0.f,  0.f,
			0.f,  0.f,  1.f,  0.f,
			in.x, in.y, in.z, 1.f,
		};
	}

	inline Matrix4 Matrix4::Scale(const float rhs)
	{
		return Matrix4::Scale(Vector3(rhs));
	}

	inline Matrix4 Matrix4::Scale(const Vector3& in)
	{
		return
		{
			in.x, 0.f,  0.f,  0.f,
			0.f,  in.y, 0.f,  0.f,
			0.f,  0.f,  in.z, 0.f,
			0.f,  0.f,  0.f,  1.f,
		};
	}

	inline Matrix4 Matrix4::RotateZXY(const Vector3& eulerAngles)
	{
		return RotateZ(eulerAngles.roll) *
			   RotateX(eulerAngles.pitch) *
			   RotateY(eulerAngles.yaw);
	}

	inline Matrix4 Matrix4::RotateX(const float rad)
	{
		float cos = std::cos(rad);
		float sin = std::sin(rad);

		return Matrix4::RotateX(cos, sin);
	}

	inline Matrix4 Matrix4::RotateY(const float rad)
	{
		float cos = std::cos(rad);
		float sin = std::sin(rad);

		return Matrix4::RotateY(cos, sin);
	}

	inline Matrix4 Matrix4::RotateZ(const float rad)
	{
		float cos = std::cos(rad);
		float sin = std::sin(rad);

		return Matrix4::RotateZ(cos, sin);
	}

	inline Matrix4 Matrix4::RotateX(const float cos, const float sin)
	{
		return
		{
			1.f, 0.f, 0.f, 0.f,
			0.f, cos, sin, 0.f,
			0.f,-sin, cos, 0.f,
			0.f, 0.f, 0.f, 1.f,
		};
	}

	inline Matrix4 Matrix4::RotateY(const float cos, const float sin)
	{
		return
		{
			cos, 0.f,-sin, 0.f,
			0.f, 1.f, 0.f, 0.f,
			sin, 0.f, cos, 0.f,
			0.f, 0.f, 0.f, 1.f,
		};
	}

	inline Matrix4 Matrix4::RotateZ(const float cos, const float sin)
	{
		return
		{
			cos, sin, 0.f, 0.f,
		   -sin, cos, 0.f, 0.f,
			0.f, 0.f, 1.f, 0.f,
			0.f, 0.f, 0.f, 1.f,
		};
	}

	inline Matrix4 Matrix4::Frustum(const float Left, const float Right, const float Bottom, const float Top, const float Near, const float Far)
	{
		return
		{
			(Near * 2.f) / (Right - Left),   0.f,                              0.f,                               0.f,
			0.f,                             (Near * 2.f) / (Top - Bottom),  0.f,                               0.f,
			(Right + Left) / (Right - Left), (Top + Bottom) / (Top - Bottom), -(Far + Near) / (Far - Near),      -1.f,
			0.f,                             0.f,                             -(Far * Near * 2.f) / (Far - Near), 0.f
		};
	}

	inline Matrix4 Matrix4::Perspective(const float FovY, const float Aspect, const float Near, const float Far)
	{
		float Top = Near * tanf(FovY * 0.5f);
		float Right = Top * Aspect;
		return Matrix4::Frustum(-Right, Right, -Top, Top, Near, Far);
	}
}