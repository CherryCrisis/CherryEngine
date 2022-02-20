#pragma once

#include "matrix4.hpp"

namespace CCMaths
{
	struct Quaternion
	{
		union
		{
			struct { float x, y, z, w; };
			float data[4];

			Vector3 xyz;
		};

		Quaternion()
			: x(0.f), y(0.f), z(0.f), w(1.f) {}

		Quaternion(const float w, const Vector3& xyz)
			: xyz(xyz)
		{
			this->w = w;
		}

		Quaternion(const float w, const float x, const float y, const float z)
			: x(x), y(y), z(z), w(w) {}

		Quaternion(const Vector4& v)
			: x(v.x), y(v.y), z(v.z), w(v.w) {}

		// Add the two Quaternions
		inline Quaternion operator+(const Quaternion& rhs) const;
		inline Quaternion& operator+=(const Quaternion& rhs);

		// Add the first Quaternion to the Conjugate of the second Quaternion
		inline Quaternion operator-(const Quaternion& rhs) const;
		inline Quaternion& operator-=(const Quaternion& rhs);

		// Multiply the two Quaternions
		inline Quaternion operator*(const Quaternion& rhs) const;
		inline Quaternion& operator*=(const Quaternion& rhs);
		
		inline Vector3 operator*(const Vector3& rhs) const;

		// Mutliply the Quaternion with the scalar
		inline Quaternion operator*(const float rhs) const;
		inline Quaternion& operator*=(const float rhs);

		// Mutliply the Quaternion with the inverse of the scalar
		inline Quaternion operator/(const Quaternion& rhs) const;
		inline Quaternion& operator/=(const Quaternion& rhs);
		inline Quaternion operator/(const float rhs) const;
		inline Quaternion& operator/=(const float rhs);

		inline bool operator==(const Quaternion& rhs) const;
		inline bool operator!=(const Quaternion& rhs) const;

		// The dot product between two rotations.
		inline float Dot(const Quaternion& rhs) const;
		inline static float Dot(const Quaternion& lhs, const Quaternion& rhs);

		inline virtual float SquareNorm() const;
		inline virtual float Norm() const;

		// Normalize the Quaternion and return the reference
		inline Quaternion& Normalize();
		inline static Quaternion& Normalize(Quaternion& outQuaternion);

		// Returns a normalized copy of the Quaternion
		inline Quaternion Normalized() const;
		inline static Quaternion Normalized(const Quaternion& inQuaternion);

		// Returns a conjugated copy of the Quaternion
		inline Quaternion operator-() const;
		inline Quaternion Conjugate() const;
		inline static Quaternion Conjugate(const Quaternion& inQuaternion);

		// Returns a inverted copy of the Quaternion
		inline Quaternion Invert() const;
		inline static Quaternion Invert(const Quaternion& inQuaternion);

		// Returns the euler angles of the rotation Quaternion (pitch, yaw, roll)
		inline Vector3 ToEuler() const;
		inline static Vector3 ToEuler(const Quaternion& inQuaternion);

		// Returns a rotation Matrix from the Quaternion
		inline Matrix4 ToMatrix() const;
		inline static Matrix4 ToMatrix(const Quaternion& inQuaternion);

		// Returns the angle and the axis of rotation
		inline void ToAxisAngle(Vector3& outAxis, float& outAngle) const;
		inline static void ToAxisAngle(const Quaternion& inQuaternion, Vector3& outAxis, float& outAngle);
		
		// Returns a Quaternion from euler angles.
		// (roll degrees around z axis, pitch degrees around x axis, yaw degrees around y axis)
		inline static Quaternion FromEuler(const float roll, const float pitch, const float yaw);
		inline static Quaternion FromEuler(const Vector3& eulerAngles);

		// Creates a Quaternion which rotates angle degrees around axis.
		inline static Quaternion FromAxisAngle(const Vector3& inAxis, const float inAngle);

		// Creates a Quaternion with a rotation Matrix
		inline static Quaternion FromMatrix(const Matrix4& inMatrix);

		// Creates a Quaternion which rotates from fromDirection to toDirection.
		inline static Quaternion FromToRotation(const Vector3& fromDirection, const Vector3& toDirection);

		inline Vector3 RotateVector3(const Vector3& inVector) const;
		inline static Vector3 RotateVector3(const Quaternion& inQuaternion, const Vector3& inVector);

		inline static Quaternion Lerp(const Quaternion& lhs, const Quaternion& rhs, const float lambda);
		inline static Quaternion NLerp(const Quaternion& lhs, const Quaternion& rhs, const float lambda);
		inline static Quaternion SLerp(const Quaternion& lhs, const Quaternion& rhs, const float lambda);
	};
}

#include "quaternion.inl"