#include <cmath>


namespace CCMaths
{
	inline Quaternion Quaternion::operator+(const Quaternion& rhs) const
	{
		return { w + rhs.w, x + rhs.x, y + rhs.y, z + rhs.z };
	}

	inline Quaternion& Quaternion::operator+=(const Quaternion& rhs)
	{
		*this = *this + rhs;
		return *this;
	}

	inline Quaternion Quaternion::operator-() const
	{
		return Quaternion::Conjugate(*this);
	}

	inline Quaternion Quaternion::operator-(const Quaternion& rhs) const
	{
		return *this + (-rhs);
	}

	inline Quaternion& Quaternion::operator-=(const Quaternion& rhs)
	{
		*this = *this - rhs;
		return *this;
	}

	inline Quaternion Quaternion::operator*(const Quaternion& rhs) const
	{
		return
		{
			w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z, // w
			w * rhs.x + x * rhs.w + y * rhs.z - z * rhs.y, // x
			w * rhs.y - x * rhs.z + y * rhs.w + z * rhs.x, // y
			w * rhs.z + x * rhs.y - y * rhs.x + z * rhs.w  // z
		};
	}

	inline Quaternion& Quaternion::operator*=(const Quaternion& rhs)
	{
		*this = *this * rhs;
		return *this;
	}

	inline Quaternion Quaternion::operator*(const float rhs) const
	{
		return { w * rhs, x * rhs, y * rhs, z * rhs };
	}

	inline Quaternion& Quaternion::operator*=(const float rhs)
	{
		*this = *this * rhs;
		return *this;
	}

	inline Vector3 Quaternion::operator*(const Vector3& rhs) const
	{
		return Quaternion::RotateVector3(*this, rhs);
	}

	inline Quaternion Quaternion::operator/(const Quaternion& rhs) const
	{
		return { w / rhs.w, x / rhs.x, y / rhs.y, z / rhs.z };
	}

	inline Quaternion& Quaternion::operator/=(const Quaternion& rhs)
	{
		*this = *this / rhs;
		return *this;
	}

	inline Quaternion Quaternion::operator/(const float rhs) const
	{
		return { w / rhs, x / rhs, y / rhs, z / rhs };
	}

	inline Quaternion& Quaternion::operator/=(const float rhs)
	{
		*this = *this / rhs;
		return *this;
	}

	inline bool Quaternion::operator==(const Quaternion& rhs) const
	{
		return (Abs(w - rhs.w) <= epsilon && 
				Abs(x - rhs.x) <= epsilon && 
				Abs(y - rhs.y) <= epsilon && 
				Abs(z - rhs.z) <= epsilon);
	}

	inline bool Quaternion::operator!=(const Quaternion& rhs) const
	{
		return !(*this == rhs);
	}
	
	inline float Quaternion::Dot(const Quaternion& rhs) const
	{
		return Quaternion::Dot(*this, rhs);
	}

	inline float Quaternion::Dot(const Quaternion& lhs, const Quaternion& rhs)
	{
		return lhs.w * rhs.w + lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
	}

	inline float Quaternion::SquareNorm() const
	{
		return this->Dot(*this);
	}

	inline float Quaternion::Norm() const
	{
		return 1.f / InverseSqrt(this->SquareNorm());
	}

	inline Quaternion& Quaternion::Normalize()
	{
		return Quaternion::Normalize(*this);
	}

	inline Quaternion& Quaternion::Normalize(Quaternion& outQuaternion)
	{
		outQuaternion = Normalized(outQuaternion);
		return outQuaternion;
	}

	inline Quaternion Quaternion::Normalized() const
	{
		return Quaternion::Normalized(*this);
	}

	inline Quaternion Quaternion::Normalized(const Quaternion& inQuaternion)
	{
		float norm = inQuaternion.SquareNorm();
		norm = norm == 0.0f ? 1.f : norm;

		return inQuaternion * InverseSqrt(norm);
	}

	inline Quaternion Quaternion::Conjugate() const
	{
		return Quaternion::Conjugate(*this);
	}

	inline Quaternion Quaternion::Conjugate(const Quaternion& inQuaternion)
	{
		return { inQuaternion.w, -inQuaternion.xyz };
	}

	inline Quaternion Quaternion::Invert() const
	{
		return Quaternion::Invert(*this);
	}

	inline Quaternion Quaternion::Invert(const Quaternion& inQuaternion)
	{
		float squareNorm = inQuaternion.SquareNorm();

		float invSquareNorm = 1.0f / (squareNorm == 0.f ? 1.f : squareNorm);
		return inQuaternion.Conjugate() * invSquareNorm;
	}

	inline Vector3 Quaternion::ToEuler() const
	{
		return Quaternion::ToEuler(*this);
	}

	inline Vector3 Quaternion::ToEuler(const Quaternion& in)
	{
		return
		{
			std::atan2f(2.f * (in.w * in.x + in.y * in.z), 1.f - 2.f * (in.x * in.x + in.y * in.y)),
			std::asinf(2.f * (in.w * in.y - in.z * in.x)),
			std::atan2f(2.f * (in.w * in.z + in.x * in.y), 1.f - 2.f * (in.y * in.y + in.z * in.z))
		};
	}

	inline Matrix4 Quaternion::ToMatrix() const
	{
		return Quaternion::ToMatrix(*this);

	}

	inline Matrix4 Quaternion::ToMatrix(const Quaternion& in)
	{
		Matrix4 result = Matrix4::Identity;

		float x2 = 2 * in.x * in.x;
		float y2 = 2 * in.y * in.y;
		float z2 = 2 * in.z * in.z;
		float xz = 2 * in.x * in.z;
		float xy = 2 * in.x * in.y;
		float yz = 2 * in.y * in.z;
		float wx = 2 * in.w * in.x;
		float wy = 2 * in.w * in.y;
		float wz = 2 * in.w * in.z;

		result.data[0] = 1 - y2 - z2;
		result.data[4] = xy - wz;
		result.data[8] = xz + wy;
		result.data[1] = xy + wz;
		result.data[5] = 1 - x2 - z2;
		result.data[9] = yz - wx;
		result.data[2] = xz - wy;
		result.data[6] = yz + wx;
		result.data[10] = 1 - x2 - y2;

		return result;
	}

	inline void Quaternion::ToAxisAngle(Vector3& outAxis, float& outAngle) const
	{
		return Quaternion::ToAxisAngle(*this, outAxis, outAngle);
	}

	inline void Quaternion::ToAxisAngle(const Quaternion& in, Vector3& outAxis, float& outAngle)
	{
		Quaternion unit = in.Normalized();

		outAngle = 2.0f * std::acosf(unit.w);
		float den = sqrtf(1.0f - unit.w * unit.w);

		if (den > 0.0001f)
		{
			outAxis.x = unit.x / den;
			outAxis.y = unit.y / den;
			outAxis.z = unit.z / den;
		}
		else
		{
			// Angle is 0
			outAxis =  Vector3::XAxis;
		}
	}

	inline Quaternion Quaternion::FromEuler(const float roll, const float pitch, const float yaw)
	{
		float rollCos  = std::cosf(roll	 * 0.5f);
		float rollSin  = std::sinf(roll	 * 0.5f);
		float pitchCos = std::cosf(pitch * 0.5f);
		float pitchSin = std::sinf(pitch * 0.5f);
		float yawCos   = std::cosf(yaw   * 0.5f);
		float yawSin   = std::sinf(yaw   * 0.5f);

		return
		{
			yawCos * pitchCos * rollCos + yawSin * pitchSin * rollSin,
			yawCos * pitchSin * rollCos - yawSin * pitchCos * rollSin,
			yawSin * pitchCos * rollCos + yawCos * pitchSin * rollSin,
			yawCos * pitchCos * rollSin - yawSin * pitchSin * rollCos
		};
	}

	inline Quaternion Quaternion::FromEuler(const Vector3& eulerAngles)
	{
		return Quaternion::FromEuler(eulerAngles.roll, eulerAngles.pitch, eulerAngles.yaw);
	}

	inline Quaternion Quaternion::FromAxisAngle(const Vector3& inAxis, const float inAngle)
	{
		float axisLength = inAxis.Length();

		if (axisLength != 0.0f)
		{
			float angle = inAngle * 0.5f;

			Vector3 unitAxis = inAxis.Normalized();

			float sin = sinf(angle);
			float cos = cosf(angle);

			Quaternion out = Quaternion(cos, unitAxis.x * sin, unitAxis.y * sin, unitAxis.z * sin);

			return out.Normalized();
		}

		return Quaternion::Identity;
	}

	inline Quaternion Quaternion::FromMatrix(const Matrix4& in)
	{
		float tr = in.data[0] + in.data[5] + in.data[10];

		if (tr > 0)
		{
			float s = sqrtf(1.0f + tr) * 2;

			return
			{
				0.25f * s,						// w
				(in.data[6] - in.data[9]) / s,	// x
				(in.data[8] - in.data[2]) / s,	// y
				(in.data[1] - in.data[4]) / s	// z
			};
		}
		else if ((in.data[0] > in.data[5]) && (in.data[0] > in.data[10]))
		{
			float s = sqrtf(1.0f + in.data[0] - in.data[5] - in.data[10]) * 2;

			return
			{
				(in.data[6] - in.data[9]) / s,	// w
				0.25f * s,						// x
				(in.data[4] + in.data[1]) / s,	// y
				(in.data[8] + in.data[2]) / s	// z
			};
		}
		else if (in.data[5] > in.data[10])
		{
			float s = sqrtf(1.0f + in.data[5] - in.data[0] - in.data[10]) * 2;

			return
			{
				(in.data[8] - in.data[2]) / s,	// w
				(in.data[4] + in.data[1]) / s,	// x
				0.25f * s,						// y
				(in.data[9] + in.data[6]) / s	// z
			};
		}
		else
		{
			float s = sqrtf(1.0f + in.data[10] - in.data[0] - in.data[5]) * 2;

			return
			{
				(in.data[1] - in.data[4]) / s,	// w
				(in.data[8] + in.data[2]) / s,	// x
				(in.data[9] + in.data[6]) / s,	// y
				0.25f * s						// z
			};
		}
	}

	inline Quaternion Quaternion::FromToRotation(const Vector3& from, const Vector3& to)
	{
		float cos2Theta = from * to;    // Dot(from, to)
		Vector3 cross = from ^ to;		// Cross(from, to)

		return Quaternion(1.0f + cos2Theta, cross).Normalized();
	}

	inline Vector3 Quaternion::RotateVector3(const Vector3& inVector) const
	{
		return Quaternion::RotateVector3(*this, inVector);
	}

	inline Vector3 Quaternion::RotateVector3(const Quaternion& in, const Vector3& inV)
	{
		return
		{
			inV.x * (in.x * in.x + in.w * in.w - in.y * in.y - in.z * in.z) + inV.y * (2 * in.x * in.y - 2 * in.w * in.z) + inV.z * (2 * in.x * in.z + 2 * in.w * in.y),
			inV.x * (2 * in.w * in.z + 2 * in.x * in.y) + inV.y * (in.w * in.w - in.x * in.x + in.y * in.y - in.z * in.z) + inV.z * (-2 * in.w * in.x + 2 * in.y * in.z),
			inV.x * (-2 * in.w * in.y + 2 * in.x * in.z) + inV.y * (2 * in.w * in.x + 2 * in.y * in.z) + inV.z * (in.w * in.w - in.x * in.x - in.y * in.y + in.z * in.z)

		};
	}

	inline Quaternion Quaternion::Lerp(const Quaternion& lhs, const Quaternion& rhs, const float lambda)
	{
		return
		{
			lhs.w + lambda * (rhs.w - lhs.w),
			lhs.x + lambda * (rhs.x - lhs.x),
			lhs.y + lambda * (rhs.y - lhs.y),
			lhs.z + lambda * (rhs.z - lhs.z)
		};
	}

	inline Quaternion Quaternion::NLerp(const Quaternion& lhs, const Quaternion& rhs, const float lambda)
	{
		return Quaternion::Lerp(lhs, rhs, lambda).Normalized();
	}

	inline Quaternion Quaternion::SLerp(const Quaternion& lhs, const Quaternion& rhs, const float lambda)
	{
		float cosHalfTheta = Quaternion::Dot(lhs, rhs);
		Quaternion negRhs = rhs;

		if (cosHalfTheta < 0.f)
		{
			negRhs.x = -rhs.x; negRhs.y = -rhs.y; negRhs.z = -rhs.z; negRhs.w = -rhs.w;
			cosHalfTheta = -cosHalfTheta;
		}

		cosHalfTheta = fabsf(cosHalfTheta);

		if (cosHalfTheta >= 1.0f)
			return lhs;

		if (cosHalfTheta > 0.95f)
			return NLerp(lhs, negRhs, lambda);

		const float halfTheta = acosf(cosHalfTheta);
		const float oneOverSinHalfTheta = 1.f / sqrtf(1.0f - cosHalfTheta * cosHalfTheta);

		const float phi = lambda * halfTheta;

		const float ratioA = sinf(halfTheta - phi) * oneOverSinHalfTheta;
		const float ratioB = sinf(phi) * oneOverSinHalfTheta;

		return  lhs * ratioA + negRhs * ratioB;
	}
}