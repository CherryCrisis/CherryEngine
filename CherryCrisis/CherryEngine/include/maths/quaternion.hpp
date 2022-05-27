#pragma once

#include "matrix4.hpp"
#include "maths_utils.hpp"

namespace CCMaths
{
	struct Quaternion
	{
		union
		{
			//! Struct of the four components of the Quaternion
			struct
			{
				float x, y, z;	//!< Complex components of the Quaternion
				float w;		//!< Scalar component of the Quaternion
			};

			float data[4];		//!< Pointer on the Quaternion's data

			Vector3 xyz;		//!< Quaternion complex components as a Vector3 
		};

		CCENGINE_API const static Quaternion Identity;		//!< { w = 1, x = 0, y = 0, z = 0 }

		//! Construct the Quaternion as the \b Identity Quaternion
		Quaternion()
			: x(0.f), y(0.f), z(0.f), w(1.f) {}

		//! Construct the Quaternion with the four given members
		/*! 
		*	\param w : Scalar component
		*	\param x : Complex component X
		*	\param y : Complex component Y
		*	\param z : Complex component Z
		*
		*	\note The scalar is place first unlike in the struct
		*/
		Quaternion(const float w, const float x, const float y, const float z)
			: x(x), y(y), z(z), w(w) {}

		//! Construct Quaternion from a scalar and a Vector3
		/*!
		*	\param w: Scalar component
		*	\param xyz: Complex components as Vector3
		*/
		Quaternion(const float w, const Vector3& xyz)
			: xyz(xyz)
		{
			this->w = w;
		}

		//! Constuct Quaternion from a Vector4
		Quaternion(const Vector4& v)
			: x(v.x), y(v.y), z(v.z), w(v.w) {}

		//! Add the two Quaternions
		/*!
		*	\param object: Quaternion on the left side of the operator
		*	\param rhs: Quaternion on the right side of the operator
		* 
		*	Create a Quaternion \b out equals to the sum of \b object and \b rhs
		* 
		*	\return Quaternion \b out
		* 
		*	\sa operator+=(const Quaternion&)
		*/
		inline Quaternion operator+(const Quaternion& rhs) const;
		
		//! Add the two Quaternions
		/*!
		*	\param object: Quaternion on the left side of the operator
		*	\param rhs: Quaternion on the right side of the operator
		*
		*	Add \b rhs to \b object
		* 
		*	\return Reference to \b object
		* 
		*	\sa operator+(const Quaternion&) const
		*/
		inline Quaternion& operator+=(const Quaternion& rhs);

		//! Conjugate the Quaternion
		/*!
		*	\param object: Quaternion from which the method is called
		*
		*	Use as \b -object
		*	Create a new Quaternion \b out as the conjugate of \b object
		* 
		*	\return Quaternion \b out
		*	
		*	\sa Conjugate() const
		*	\sa Conjugate(const Quaternion&)
		*/
		inline Quaternion operator-() const;

		//! Add the first Quaternion and the Conjugate of the second
		/*!
		*	\param object: Quaternion on the left side of the operator
		*	\param rhs: Quaternion on the right side of the operator
		*
		*	Create a Quaternion \b out equals to the sum of \b object and the conjugate of \b rhs
		*
		*	\return Quaternion \b out
		* 
		*	\sa operator-=(const Quaternion&)
		*/
		inline Quaternion operator-(const Quaternion& rhs) const;

		//! Add the first Quaternion and the Conjugate of the second 
		/*!
		*	\param object: Quaternion on the left side of the operator
		*	\param rhs: Quaternion on the right side of the operator
		*
		*	Add the conjugate of \b rhs to \b object
		*
		*	\return Reference to \b object
		* 
		*	\sa operator-(const Quaternion&) const
		*/
		inline Quaternion& operator-=(const Quaternion& rhs);

		//! Multiply the two Quaternions
		/*!
		*	\param object: Quaternion on the left side of the operator
		*	\param rhs: Quaternion on the right side of the operator
		*
		*	Create a Quaternion \b out equals to the product of \b object and \b rhs
		*
		*	\return Quaternion \b out
		* 
		*	\sa operator*=(const Quaternion&)
		*	\sa operator=(const float) const
		*	\sa operator*=(const float)
		*/
		inline Quaternion operator*(const Quaternion& rhs) const;

		//! Multiply the two Quaternions
		/*!
		*	\param object: Quaternion on the left side of the operator
		*	\param rhs: Quaternion on the right side of the operator
		*
		*	Mutliply \b object by \b rhs
		*
		*	\return Reference to \b object
		* 
		*	\sa operator*(const Quaternion&) const
		*/
		inline Quaternion& operator*=(const Quaternion& rhs);
		
		//! Rotate the Vector3 by the Quaternion
		/*!
		*	\param object: Quaternion on the left side of the operator
		*	\param rhs: Vector3 on the right side of the operator
		*
		*	Create a Vector3 \b out equals to \b rhs rotated by \b object
		*
		*	\return Vector3 \b out
		* 
		*	\sa RotateVector3(const Vector3&) const
		*	\sa RotateVector3(const Quaternion&, const Vector3&)
		*/
		inline Vector3 operator*(const Vector3& rhs) const;

		//! Mutliply the Quaternion by the scalar
		/*!
		*	\param object: Quaternion on the left side of the operator
		*	\param rhs: float on the right side of the operator
		*
		*	Create a Quaternion \b out equals to the product of \b object by \b rhs
		*
		*	\return Quaternion \b out
		* 
		*	\sa operator*=(const float)
		*	\sa operator*(const Quaternion&) const
		*	\sa operator*=(const Quaternion&)
		*/
		inline Quaternion operator*(const float rhs) const;

		//! Mutliply the Quaternion by the scalar
		/*!
		*	\param object : Quaternion on the left side of the operator
		*	\param rhs : float on the right side of the operator
		*
		*	Mutliply \b object by \b rhs
		*
		*	\return Reference to \b object
		*	
		*	\sa operator*(const float) const
		*	\sa operator*=(const Quaternion&)
		*	\sa operator*(const Quaternion&) const
		*/
		inline Quaternion& operator*=(const float rhs);
		
		//! Divise the first Quaternion by the second
		/*!
		*	\param object: Quaternion on the left side of the operator
		*	\param rhs: Quaternion on the right side of the operator
		* 
		*	Create a Quaternion \b out equals to the division of each member \b object by the corresponding member of \b rhs
		*
		*	\return Quaternion \b out
		* 		
		*	\sa operator/=(const Quaternion&)
		*	\sa operator=(const float) const
		*	\sa operator/=(const float)
		*/
		inline Quaternion operator/(const Quaternion& rhs) const;

		//! Divise the first Quaternion by the second
		/*!
		*	\param object : Quaternion on the left side of the operator
		*	\param rhs : Quaternion on the right side of the operator
		*
		*	Divide each member \b object by the corresponding member of \b rhs
		*
		*	\return Reference to \b object
		* 		
		*	\sa operator=(const Quaternion&) const
		*	\sa operator/=(const float)
		*	\sa operator=(const float) const
		*/
		inline Quaternion& operator/=(const Quaternion& rhs);

		//! Mutliply the Quaternion by the inverse of the scalar
		/*!
		*	\param object: Quaternion on the left side of the operator
		*	\param rhs: float on the right side of the operator
		*
		*	Create a Quaternion \b out equals to the division of \b object by \b rhs
		*
		*	\return Quaternion \b out
		*
		*	\sa operator/=(const float)
		*	\sa operator=(const Quaternion&) const
		*	\sa operator/=(const Quaternion&)
		*/
		inline Quaternion operator/(const float rhs) const;
		
		//! Mutliply the Quaternion by the inverse of the scalar
		/*!
		*	\param object : Quaternion on the left side of the operator
		*	\param rhs : float on the right side of the operator
		*
		*	Divide \b object by \b rhs
		*
		*	\return Reference to \b object
		* 
		*	\sa operator=(const float) const
		*	\sa operator/=(const Quaternion&)
		*	\sa operator/(const Quaternion&) const
		*/
		inline Quaternion& operator/=(const float rhs);

		//! Compare the two Quaternions
		/*!
		*	\param object : Quaternion on the left side of the operator
		*	\param rhs : Quaternion on the right side of the operator
		*
		*	Check wether each member \b object equals the corresponding member of \b rhs
		*
		*	\return \b True if all checks are true
		* 
		*	\sa operator!=(const Quaternion&) const
		*/
		inline bool operator==(const Quaternion& rhs) const;

		//! Compare the two Quaternions
		/*!
		*	\param object : Quaternion on the left side of the operator
		*	\param rhs : Quaternion on the right side of the operator
		*
		*	Check wether each member \b object equals the corresponding member of \b rhs
		*
		*	\return \b True if at least one check is false
		* 
		*	\sa operator==(const Quaternion&) const
		*/
		inline bool operator!=(const Quaternion& rhs) const;

		//! The dot product between two Quaternions
		/*!
		*	\attention Use as a member method of a specific object
		* 
		*	\param object: Quaternion from which the method is called
		*	\param rhs: Quaternion given as parameter
		* 
		*	Create a new float \b out equals to the dot product of \b object and \b rhs
		* 
		*	\return float \b out
		* 
		*	\note Has the same result as the non-object specific static version: Dot(const Quaternion&, const Quaternion&)
		* 
		*	\sa Dot(const Quaternion&, const Quaternion&)
		*/
		inline float Dot(const Quaternion& rhs) const;

		//! The dot product between two Quaternions
		/*!
		*	\attention Use as an independant method of the struct (not relative to a specific object)
		*
		*	\param lhs: Quaternion given as parameter
		*	\param rhs: Quaternion given as parameter
		*
		*	Create a new float \b out equals to the dot product of \b object and \b rhs
		*
		*	\return float \b out
		* 
		*	\note Has the same result as the object specific version: Dot(const Quaternion&) const
		* 
		*	\sa Dot(const Quaternion&) const
		*/
		inline static float Dot(const Quaternion& lhs, const Quaternion& rhs);

		//! Square Norm of the specific Quaternion
		/*!
		*	\attention Use as a member method of a specific object
		*
		*	\param object: Quaternion from which the method is called
		*
		*	Create a new float \b out equals to the square norm of \b object
		*
		*	\return float \b out
		*
		*	\sa Norm() const
		*/
		inline float SquareNorm() const;

		//! Norm of the specific Quaternion
		/*!
		*	\attention Use as a member method of a specific object
		*
		*	\param object: Quaternion from which the method is called
		*
		*	Create a new float \b out equals to the norm of \b object
		*
		*	\return float \b out
		* 
		*	\sa SquareNorm() const
		*/
		inline float Norm() const;

		//! Normalize the Quaternion
		/*!
		*	\attention Use as a member method of a specific object
		*
		*	\param object: Quaternion from which the method is called
		*
		*	Normalize \b object
		*
		*	\return Reference to \b object
		*
		*	\note Has the same result as the non-object specific static version: Normalize(Quaternion&)
		* 
		*	\note Use Normalized() if you \b don't want to modify \b object
		* 
		*	\sa Normalize(Quaternion&)
		*	\sa Normalized() const
		*	\sa Normalized(const Quaternion&)
		*/
		inline Quaternion& Normalize();

		//! Normalize the Quaternion
		/*!
		*	\attention Use as an independant method of the struct (not relative to a specific object)
		*
		*	\param outQuaternion: Quaternion given as parameter
		*
		*	Normalize \b outQuaternion
		*
		*	\return Reference to \b outQuaternion
		*
		*	\note Has the same result as the object specific version: Normalize()
		* 
		*	\note Use Normalized() if you \b don't want to modify \b outQuaternion
		* 
		*	\sa Normalize()
		*	\sa Normalized(const Quaternion&)
		*	\sa Normalized() const
		*/
		inline static Quaternion& Normalize(Quaternion& outQuaternion);

		//! Normalize the Quaternion
		/*!
		*	\attention Use as a member method of a specific object
		*
		*	\param object: Quaternion from which the method is called
		*
		*	Create a new Quaternion \b out equals to normalized \b object
		*
		*	\return Quaternion \b out
		*
		*	\note Has the same result as the non-object specific static version: Normalized(const Quaternion&)
		* 
		*	\note Use Normalize() if you want to modify \b object
		* 
		*	\sa Normalized(const Quaternion&)
		*	\sa Normalize()
		*	\sa Normalize(Quaternion&)
		*/
		inline Quaternion Normalized() const;

		//! Normalize the Quaternion
		/*!
		*	\attention Use as an independant method of the struct (not relative to a specific object)
		*
		*	\param inQuaternion: Quaternion given as parameter
		*
		*	Create a new Quaternion \b out equals to normalized \b inQuaternion
		*
		*	\return Quaternion \b out
		*
		*	\note Has the same result as the object specific version: Normalized() const
		* 
		*	\note Use Normalize() if you want to modify \b inQuaternion
		* 
		*	\sa Normalized() const
		*	\sa Normalize(Quaternion&)
		*	\sa Normalize()
		*/
		inline static Quaternion Normalized(const Quaternion& inQuaternion);

		//! Conjugate the Quaternion
		/*!
		*	\attention Use as a member method of a specific object
		*
		*	\param object: Quaternion from which the method is called
		*
		*	Create a new Quaternion \b out as the conjugate of \b object
		*
		*	\return Quaternion \b out
		*
		*	\note Has the same result as the non-object specific static version: Conjugate(const Quaternion&)
		* 
		*	\sa Conjugate(const Quaternion&)
		*	\sa operator-() const
		*/
		inline Quaternion Conjugate() const;

		//! Conjugate the Quaternion
		/*!
		*	\attention Use as an independant method of the struct (not relative to a specific object)
		*
		*	\param inQuaternion: Quaternion given as parameter
		*
		*	Create a new Quaternion \b out as the conjugate of \b inQuaternion
		*
		*	\return Quaternion \b out
		*
		*	\note Has the same result as the object specific version: Conjugate() const
		* 
		*	\sa Conjugate() const
		*	\sa operator-() const
		*/
		inline static Quaternion Conjugate(const Quaternion& inQuaternion);

		//! Inverse the Quaternion
		/*!
		*	\attention Use as a member method of a specific object
		*
		*	\param object: Quaternion from which the method is called
		*
		*	Create a new Quaternion \b out equals to the inverted \b object
		*
		*	\return Quaternion \b out
		*
		*	\note Has the same result as the non-object specific static version: Invert(const Quaternion&)
		* 
		*	\sa Invert(const Quaternion&)
		*/
		inline Quaternion Invert() const;

		//! Inverse the Quaternion
		/*!
		*	\attention Use as an independant method of the struct (not relative to a specific object)
		*
		*	\param inQuaternion: Quaternion given as parameter
		*
		*	Create a new Quaternion \b out equals to the inverted \b inQuaternion
		*
		*	\return Quaternion \b out
		*
		*	\note Has the same result as the object specific version: Invert() const
		* 
		*	\sa Invert() const
		*/
		inline static Quaternion Invert(const Quaternion& inQuaternion);

		//! Euler angles of the Quaternion
		/*!
		*	\attention Use as a member method of a specific object
		*
		*	\param object: Quaternion from which the method is called
		*
		*	Create a new Vector3 \b out of euler angles in radians, corresponding to the rotation of \b object
		*	Vector3 \b out = { roll, pitch, yaw };
		*
		*	\return Vector3 \b out
		*
		*	\note Has the same result as the non-object specific static version: ToEuler(const Quaternion&)
		* 
		*	\sa ToEuler(const Quaternion&)
		*	\sa FromEuler(const float, const float, const float)
		*	\sa FromEuler(const Vector3&)
		*/
		inline Vector3 ToEuler() const;

		//! Euler angles of the Quaternion
		/*!
		*	\attention Use as an independant method of the struct (not relative to a specific object)
		*
		*	\param inQuaternion: Quaternion given as parameter
		*
		*	Create a new Vector3 \b out of euler angles in radians, corresponding to the rotation of \b inQuaternion
		*	Vector3 \b out = { roll, pitch, yaw };
		*
		*	\return Vector3 \b out
		*
		*	\note Has the same result as the object specific version: ToEuler() const
		* 
		*	\sa ToEuler() const
		*	\sa FromEuler(const Vector3&)
		*	\sa FromEuler(const float, const float, const float)
		*/
		inline static Vector3 ToEuler(const Quaternion& inQuaternion);

		//! Rotation Matrix of the Quaternion
		/*!
		*	\attention Use as a member method of a specific object
		*
		*	\param object: Quaternion from which the method is called
		*
		*	Create a new Matrix4 \b out, corresponding to the rotation of \b object
		*
		*	\return Matrix4 \b out
		*
		*	\note Has the same result as the non-object specific static version: ToMatrix(const Quaternion&)
		* 
		*	\sa ToMatrix(const Quaternion&)
		*	\sa FromMatrix(const Matrix4&)
		*/
		inline Matrix4 ToMatrix() const;

		//! Rotation Matrix of the Quaternion
		/*!
		*	\attention Use as an independant method of the struct (not relative to a specific object)
		*
		*	\param inQuaternion: Quaternion given as parameter
		*
		*	Create a new Matrix4 \b out, corresponding to the rotation of \b inQuaternion
		*
		*	\return Matrix4 \b out
		*
		*	\note Has the same result as the object specific version: ToMatrix() const
		* 
		*	\sa ToMatrix() const
		*	\sa FromMatrix(const Matrix4&)
		*/
		inline static Matrix4 ToMatrix(const Quaternion& inQuaternion);

		//! Rotation Axis and Angle of the Quaternion
		/*!
		*	\attention Use as a member method of a specific object
		*
		*	\param object: Quaternion from which the method is called
		*	\param outAxis: Vector3 to be set, axis of rotation
		*	\param outAngle: float to be set, angle of rotation
		*
		*	Get the axis and angle of rotation from \b object and set \b outAxis and \b outAngle
		*
		*	\return void
		*
		*	\note Has the same result as the non-object specific static version: ToAxisAngle(const Quaternion&, Vector3&, float&)
		* 
		*	\sa ToAxisAngle(const Quaternion&, Vector3&, float&)
		*	\sa FromAxisAngle(const Vector3&, const float)
		*/
		inline void ToAxisAngle(Vector3& outAxis, float& outAngle) const;

		//! Rotation Axis and Angle of the Quaternion
		/*!
		*	\attention Use as an independant method of the struct (not relative to a specific object)
		*
		*	\param inQuaternion: Quaternion given as parameter
		*	\param outAxis: Vector3 to be set, axis of rotation
		*	\param outAngle: float to be set, angle of rotation
		*
		*	Get the axis and angle of rotation from \b inQuaternion and set \b outAxis and \b outAngle
		*
		*	\return void
		*
		*	\note Has the same result as the object specific version: ToAxisAngle(Vector3&, float&) const
		* 
		*	\sa ToAxisAngle(Vector3&, float&) const
		*	\sa FromAxisAngle(const Vector3&, const float)
		*/
		inline static void ToAxisAngle(const Quaternion& inQuaternion, Vector3& outAxis, float& outAngle);
		
		//! Quaternion from Euler Angles
		/*!
		*	\param roll: float given as parameter, rotation around the forward/Z axis
		*	\param pitch: float given as parameter, rotation around the right/X axis
		*	\param yaw: float given as parameter, rotation around the up/Y axis
		*
		*	Create a Quaternion \b out from the euler angles
		*
		*	\return Quaternion \b out
		*
		*	\note Has the same result as the version with Vector3 \b eulerAngles as parameter: FromEuler(const Vector3&)
		* 
		*	\sa FromEuler(const Vector3&)
		*	\sa ToEuler() const
		*	\sa ToEuler(const Quaternion&)
		*/
		inline static Quaternion FromEuler(const float roll, const float pitch, const float yaw);

		//! Quaternion from Euler Angles
		/*!
		*	\param eulerAngles: Vector3
		*
		*	Create a Quaternion \b out from the euler angles
		*
		*	\return Quaternion \b out
		*
		*	\note Has the same result as the version 3 separated parameters roll, pitch, yaw: FromEuler(const float, const float, const float)
		* 
		*	\sa FromEuler(const float, const float, const float)
		*	\sa ToEuler(const Quaternion&)
		*	\sa ToEuler() const
		*/
		inline static Quaternion FromEuler(const Vector3& eulerAngles);

		//! Quaternion from an axis and an angle of rotation
		/*!
		*	\param inAxis: Vector3 given as parameter, axis of rotation
		*	\param inAngle: float given as parameter, angle of rotation
		*
		*	Create a Quaternion \b out from the axis of rotation \b inAxis and the angle of rotation \b inAngle around the Axis
		*
		*	\return Quaternion \b out
		* 
		*	\sa ToAxisAngle(Vector3&, float&) const
		*	\sa ToAxisAngle(const Quaternion&, Vector3&, float&)
		*/
		inline static Quaternion FromAxisAngle(const Vector3& inAxis, const float inAngle);

		//! Quaternion from a rotation matrix
		/*!
		*	\param inMatrix: Matrix4 given as parameter
		*
		*	Create a Quaternion \b out corresponding to the rotation of \b inMatrix
		*
		*	\return Quaternion \b out
		* 
		*	\sa ToMatrix() const
		*	\sa ToMatrix(const Quaternion&)
		*/
		inline static Quaternion FromMatrix(const Matrix4& inMatrix);

		//! Quaternion from two direction
		/*!
		*	\param fromDirection: Vector3 given as parameter
		*	\param toDirection: Vector3 given as parameter
		*
		*	Create a Quaternion \b out corresponding to the rotation between \b fromDirection and \b toDirection 
		*
		*	\return Quaternion \b out
		*/
		inline static Quaternion FromToRotation(const Vector3& fromDirection, const Vector3& toDirection);

		//! Rotate the Vector3 rhs by the Quaternion
		/*!
		*	\attention Use as a member method of a specific object
		* 
		*	\param object: Quaternion from which the method is called
		*	\param inVector: Vector3 given as parameter
		*
		*	Create a Vector3 \b out equals to \b inVector rotated by \b object
		*
		*	\return Vector3 \b out
		*
		*	\note Has the same result as the non-object specific static version: RotateVector3(const Quaternion&, const Vector3&)
		* 
		*	\sa RotateVector3(const Quaternion&, const Vector3&)
		*	\sa operator*(const Vector3&) const
		*/
		inline Vector3 RotateVector3(const Vector3& inVector) const;

		//! Rotate the Vector3 rhs by the Quaternion
		/*!
		*	\attention Use as an independant method of the struct (not relative to a specific object)
		*
		*	\param inQuaternion: Quaternion given as parameter
		*	\param inVector: Vector3 given as parameter
		*
		*	Create a Vector3 \b out equals to \b inVector rotated by \b inQuaternion
		*
		*	\return Vector3 \b out
		*
		*	\note Has the same result as the object specific version: RotateVector3(const Vector3&) const
		* 
		*	\sa RotateVector3(const Vector3&) const
		*	\sa operator*(const Vector3&) const
		*/
		inline static Vector3 RotateVector3(const Quaternion& inQuaternion, const Vector3& inVector);

		//! Lerp of lambda between Quaternion \b lhs and Quaternion \b rhs
		/*!
		*	\param lhs: First Quaternion given as Parameter
		*	\param rhs: Second Quaternion given as Parameter
		*	\param lanbda: lerping ratio
		* 
		*	Create a new Quaternion \b out form the lerping of ratio \b lanbda between \lhs and \rhs
		* 
		*	\return Quaternion \b out
		* 
		*	\sa NLerp(const Quaternion&, const Quaternion&, const float)
		*	\sa SLerp(const Quaternion&, const Quaternion&, const float)
		*/
		inline static Quaternion Lerp(const Quaternion& lhs, const Quaternion& rhs, const float lambda);

		//! Normalized Lerp of lambda between Quaternion \b lhs and Quaternion \b rhs
		/*!
		*	\param lhs: First Quaternion given as Parameter
		*	\param rhs: Second Quaternion given as Parameter
		*	\param lanbda: lerping ratio
		*
		*	Create a new normalized Quaternion \b out form the lerping of ratio \b lanbda between \lhs and \rhs
		*
		*	\return Quaternion \b out
		*
		*	\sa Lerp(const Quaternion&, const Quaternion&, const float)
		*	\sa SLerp(const Quaternion&, const Quaternion&, const float)
		*/
		inline static Quaternion NLerp(const Quaternion& lhs, const Quaternion& rhs, const float lambda);

		//! Spherical Lerp of lambda between Quaternion \b lhs and Quaternion \b rhs
		/*!
		*	\param lhs: First Quaternion given as Parameter
		*	\param rhs: Second Quaternion given as Parameter
		*	\param lanbda: lerping ratio
		*
		*	Create a new Quaternion \b out form the spherical lerping of ratio \b lanbda between \lhs and \rhs
		*
		*	\return Quaternion \b out
		*
		*	\sa Lerp(const Quaternion&, const Quaternion&, const float)
		*	\sa NLerp(const Quaternion&, const Quaternion&, const float)
		*/
		inline static Quaternion SLerp(const Quaternion& lhs, const Quaternion& rhs, const float lambda);
	};
}

#include "quaternion.inl"