#pragma once

#include "vector2.hpp"


namespace CCMaths
{
	struct Vector3
	{
		union
		{
			struct { float x, y, z; };				//!< Struct of the three components of the Vector3
			struct { float r, g, b; };				//!< Struct of the three components of the Vector3
			struct { float pitch, yaw, roll; };		//!< Struct of the three components of the Vector3

			float data[3];							//!< Pointer on the Vector3's data

			Vector2 xy;
		};
							
		//! Construct the Vector3 as the \b Zero Vector3
		Vector3()
			:x(0.f), y(0.f), z(0.f) {}

		//! Construct the Vector3 with the three components equal to the given value
		Vector3(const float value)
			:x(value), y(value), z(value) {}

		//! Construct the Vector3 with the three given members
		Vector3(const float x, const float y, const float z)
			: x(x), y(y), z(z) {}

		//! Construct the Vector3 from a Vector2 and a float
		Vector3(const Vector2& xy, float z)
			: xy(xy)
		{
			this->z = z;
		}

		//! Add the two Vector3s
		/*!
		*	\param object: Vector3 on the left side of the operator
		*	\param rhs: Vector3 on the right side of the operator
		*
		*	Create a Vector3 \b out equals to the sum of \b object and \b rhs
		*
		*	\return Vector3 \b out
		*
		*	\sa operator+=(const Vector3&)
		*	\sa operator+(const float) const
		*	\sa operator+=(const float)
		*/
		inline Vector3 operator+(const Vector3& rhs) const;

		//! Add the two Vector3s
		/*!
		*	\param object: Vector3 on the left side of the operator
		*	\param rhs: Vector3 on the right side of the operator
		*
		*	Add \b rhs to \b object
		*
		*	\return Reference to \b object
		*
		*	\sa operator+(const Vector3&) const
		*	\sa operator+=(const float)
		*	\sa operator+(const float) const
		*/
		inline Vector3& operator+=(const Vector3& rhs);

		//! Add a scalar to the Vector3
		/*!
		*	\param object: Vector3 on the left side of the operator
		*	\param rhs: float on the right side of the operator
		*
		*	Create a Vector3 \b out by adding \b rhs and each member of \b object
		*
		*	\return Vector3 \b out
		*
		*	\sa operator+=(const float)
		*	\sa operator+(const Vector3&) const
		*	\sa operator+=(const Vector3&)
		*/
		inline Vector3 operator+(const float rhs) const;

		//! Add the two Vector3s
		/*!
		*	\param object: Vector3 on the left side of the operator
		*	\param rhs: Vector3 on the right side of the operator
		*
		*	Add \b rhs to each member of \b object
		*
		*	\return Reference to \b object
		*
		*	\sa operator+(const float) const
		*	\sa operator+=(const Vector3&)
		*	\sa operator+(const Vector3&) const
		*/
		inline Vector3& operator+=(const float rhs);

		//! Oppose the Vector3
		/*!
		*	\param object: Quaternion from which the method is called
		*
		*	Use as \b -object
		*	Create a new Vector3 \b out with each member equals to - the corresponding \b object's member
		*
		*	\return Vector3 \b out
		*/
		inline Vector3 operator-() const;

		//! Substract the two Vector3s
		/*!
		*	\param object: Vector3 on the left side of the operator
		*	\param rhs: Vector3 on the right side of the operator
		*
		*	Create a Vector3 \b out equals to the difference between \b object and \b rhs
		*
		*	\return Vector3 \b out
		*
		*	\sa operator-=(const Vector3&)
		*	\sa	operator-(const float) const
		*	\sa operator-=(const float)
		*/
		inline Vector3 operator-(const Vector3& rhs) const;

		//! Substract the two Vector3s 
		/*!
		*	\param object: Vector3 on the left side of the operator
		*	\param rhs: Vector3 on the right side of the operator
		*
		*	Substract \b rhs from \b object
		*
		*	\return Reference to \b object
		*
		*	\sa	operator-(const Vector3&) const
		*	\sa operator-=(const float)
		*	\sa operator-(const float) const
		*/
		inline Vector3& operator-=(const Vector3& rhs);

		//! Substract a scalar from the Vector3
		/*!
		*	\param object: Vector3 on the left side of the operator
		*	\param rhs: float on the right side of the operator
		*
		*	Create a Vector3 \b out equals to the difference between each member of \b object and \b rhs
		*
		*	\return Vector3 \b out
		*
		*	\sa operator-=(const float)
		*	\sa	operator-(const Vector3&) const
		*	\sa operator-=(const Vector3&)
		*/
		inline Vector3 operator-(const float rhs) const;

		//! Substract a scalar from the Vector3
		/*!
		*	\param object: Vector3 on the left side of the operator
		*	\param rhs: Vector3 on the right side of the operator
		*
		*	Substract \b rhs from each member of \b object
		*
		*	\return Reference to \b object
		*
		*	\sa operator-(const float) const
		*	\sa operator-=(const Vector3&)
		*	\sa	operator-(const Vector3&) const
		*/
		inline Vector3& operator-=(const float rhs);

		//! The dot product between two Vector3s
		/*!
		*	\attention Use as a member method of a specific object
		*
		*	\param object: Vector3 from which the method is called
		*	\param rhs: Vector3 given as parameter
		*
		*	Create a new float \b out equals to the dot product of \b object and \b rhs
		*
		*	\return float \b out
		*
		*	\sa Dot(const Vector3&) const
		*	\sa Dot(const Vector3&, const Vector3&)
		*/
		inline float operator*(const Vector3& rhs) const;

		//! Mutliply the Vector3 by the scalar
		/*!
		*	\param object: Vector3 on the left side of the operator
		*	\param rhs: float on the right side of the operator
		*
		*	Create a Vector3 \b out equals to the product of \b object by \b rhs
		*
		*	\return Vector3 \b out
		*
		*	\sa operator*=(const float)
		*	\sa operator*=(const Vector3&)
		*/
		inline Vector3 operator*(const float rhs) const;

		//! Mutliply the Vector3 by the scalar
		/*!
		*	\param object : Vector3 on the left side of the operator
		*	\param rhs : Vector3 on the right side of the operator
		*
		*	Mutliply \b object by \b rhs
		*
		*	\return Reference to \b object
		*
		*	\sa operator*=(const float)
		*	\sa operator*(const float const)
		*/
		inline Vector3& operator*=(const Vector3& rhs);

		//! Mutliply the Vector3 by the scalar
		/*!
		*	\param object : Vector3 on the left side of the operator
		*	\param rhs : float on the right side of the operator
		*
		*	Mutliply \b object by \b rhs
		*
		*	\return Reference to \b object
		*
		*	\sa operator*=(const Vector3&)
		*	\sa operator*(const float) const
		*/
		inline Vector3& operator*=(const float rhs);

		//! The cross product between two Vector3s
		/*!
		*	\attention Use as a member method of a specific object
		*
		*	\param object: Vector3 from which the method is called
		*	\param rhs: Vector3 given as parameter
		*
		*	Create a new Vector3 \b out equals to the cross product of \b object and \b rhs
		*
		*	\return Vector3 \b out
		*
		*	\sa Cross(const Vector3&) const
		*	\sa Cross(const Vector3&, const Vector3&)
		*/
		inline Vector3 operator^(const Vector3& rhs) const;

		//! Mutliply the Vector3 by the inverse of the scalar
		/*!
		*	\param object: Vector3 on the left side of the operator
		*	\param rhs: float on the right side of the operator
		*
		*	Create a Vector3 \b out equals to the division of \b object by \b rhs
		*
		*	\return Vector3 \b out
		*
		*	\sa operator/=(const float)
		*/
		inline Vector3 operator/(const float rhs) const;

		//! Mutliply the Vector3 by the inverse of the scalar
		/*!
		*	\param object : Vector3 on the left side of the operator
		*	\param rhs : float on the right side of the operator
		*
		*	Divide \b object by \b rhs
		*
		*	\return Reference to \b object
		*
		*	\sa operator/(const float) const
		*/
		inline Vector3& operator/=(const float rhs);

		//! Compare the two Vector3s
		/*!
		*	\param object : Vector3 on the left side of the operator
		*	\param rhs : Vector3 on the right side of the operator
		*
		*	Check equality of \b object and \b rhs member to member
		*
		*	\return \b True if all checks are true
		*
		*	\sa operator!=(const Vector3&) const
		*/
		inline bool operator==(const Vector3& rhs) const;

		//! Compare the two Vector3s
		/*!
		*	\param object : Vector3 on the left side of the operator
		*	\param rhs : Vector3 on the right side of the operator
		*
		*	Check equality of \b object and \b rhs member to member
		*
		*	\return \b True if at least one check is false
		*
		*	\sa operator==(const Vector3&) const
		*/
		inline bool operator!=(const Vector3& rhs) const;

		//! The dot product between two Vector3s
		/*!
		*	\attention Use as a member method of a specific object
		*
		*	\param object: Vector3 from which the method is called
		*	\param rhs: Vector3 given as parameter
		*
		*	Create a new float \b out equals to the dot product of \b object and \b rhs
		*
		*	\return float \b out
		*
		*	\note Has the same result as the non-object specific static version: Dot(const Vector3&, const Vector3&)
		*
		*	\sa Dot(const Vector3&, const Vector3&)
		*	\sa operator*(const Vector3&) const
		*/
		inline float Dot(const Vector3& rhs) const;

		//! The dot product between two Vector3s
		/*!
		*	\attention Use as an independant method of the struct (not relative to a specific object)
		*
		*	\param lhs: Vector3 given as parameter
		*	\param rhs: Vector3 given as parameter
		*
		*	Create a new float \b out equals to the dot product of \b object and \b rhs
		*
		*	\return float \b out
		*
		*	\note Has the same result as the object specific version: Dot(const Vector3&) const
		*
		*	\sa Dot(const Vector3&) const
		*	\sa operator*(const Vector3&) const
		*/
		inline static float Dot(const Vector3& lhs, const Vector3& rhs);

		inline Vector3& Multiply(const Vector3& rhs);

		inline static Vector3 Multiply(const Vector3& lhs, const Vector3& rhs);

		//! The cross product between two Vector3s
		/*!
		*	\attention Use as a member method of a specific object
		*
		*	\param object: Vector3 from which the method is called
		*	\param rhs: Vector3 given as parameter
		*
		*	Create a new Vector3 \b out equals to the cross product of \b object and \b rhs
		*
		*	\return Vector3 \b out
		*
		*	\note Has the same result as the non-object specific static version: Cross(const Vector3&, const Vector3&)
		*
		*	\sa Cross(const Vector3&, const Vector3&)
		*	\sa operator^(const Vector3&) const
		*/
		inline Vector3 Cross(const Vector3& rhs) const;

		//! The cross product between two Vector3s
		/*!
		*	\attention Use as an independant method of the struct (not relative to a specific object)
		*
		*	\param lhs: Vector3 given as parameter
		*	\param rhs: Vector3 given as parameter
		*
		*	Create a new Vector3 \b out equals to the cross product of \b object and \b rhs
		*
		*	\return Vector3 \b out
		*
		*	\note Has the same result as the object specific version: Cross(const Vector3&) const
		*
		*	\sa Cross(const Vector3&) const
		*	\sa operator^(const Vector3&) const
		*/
		inline static Vector3 Cross(const Vector3& lhs, const Vector3& rhs);

		//! Square Norm of the Vector3
		/*!
		*	\attention Use as a member method of a specific object
		*
		*	\param object: Vector3 from which the method is called
		*
		*	Create a new float \b out equals to the square norm of \b object
		*
		*	\return float \b out
		*
		*	\sa Length() const
		*/
		inline float SquareLength() const;

		//! Norm of the Vector3
		/*!
		*	\attention Use as a member method of a specific object
		*
		*	\param object: Vector3 from which the method is called
		*
		*	Create a new float \b out equals to the norm of \b object
		*
		*	\return float \b out
		*
		*	\sa SquareLength() const
		*/
		inline float Length() const;

		inline void ClampLength(float minLength, float maxLength);
		
		inline static Vector3 ClampLength(const Vector3& toClamp, float minLength, float maxLength);

		//! Normalize the Vector3
		/*!
		*	\attention Use as a member method of a specific object
		*
		*	\param object: Vector3 from which the method is called
		*
		*	Normalize \b object
		*
		*	\return Reference to \b object
		*
		*	\note Has the same result as the non-object specific static version: Normalize(Vector3&)
		*
		*	\note Use Normalized() if you \b don't want to modify \b object
		*
		*	\sa Normalize(Vector3&)
		*	\sa Normalized() const
		*	\sa Normalized(const Vector3&)
		*/
		inline Vector3& Normalize();

		//! Normalize the Vector3
		/*!
		*	\attention Use as an independant method of the struct (not relative to a specific object)
		*
		*	\param outVector: Vector3 given as parameter
		*
		*	Normalize \b outVector
		*
		*	\return Reference to \b outVector
		*
		*	\note Has the same result as the object specific version: Normalize()
		*
		*	\note Use Normalized() if you \b don't want to modify \b outVector
		*
		*	\sa Normalize()
		*	\sa Normalized(const Vector3&)
		*	\sa Normalized() const
		*/
		inline static Vector3& Normalize(Vector3& outVector);

		//! Normalize the Vector3
		/*!
		*	\attention Use as a member method of a specific object
		*
		*	\param object: Vector3 from which the method is called
		*
		*	Create a new Vector3 \b out equals to normalized \b object
		*
		*	\return Vector3 \b out
		*
		*	\note Has the same result as the non-object specific static version: Normalized(const Vector3&)
		*
		*	\note Use Normalize() if you want to modify \b object
		*
		*	\sa Normalized(const Vector3&)
		*	\sa Normalize()
		*	\sa Normalize(Vector3&)
		*/
		inline Vector3 Normalized() const;

		//! Normalize the Vector3
		/*!
		*	\attention Use as an independant method of the struct (not relative to a specific object)
		*
		*	\param inVector: Vector3 given as parameter
		*
		*	Create a new Vector3 \b out equals to normalized \b inVector
		*
		*	\return Vector3 \b out
		*
		*	\note Has the same result as the object specific version: Normalize() const
		*
		*	\note Use Normalize() if you want to modify \b inVector
		*
		*	\sa Normalized() const
		*	\sa Normalize(Vector3&)
		*	\sa Normalize()
		*/
		inline static Vector3 Normalized(const Vector3& inVector);

		CCENGINE_API static const Vector3 XAxis;				//!< { x = 1, y = 0, z = 0 }
		CCENGINE_API static const Vector3 YAxis;				//!< { x = 0, y = 1, z = 0 }
		CCENGINE_API static const Vector3 ZAxis;				//!< { x = 0, y = 0, z = 1 }
		CCENGINE_API static const Vector3 Forward;				//!< { x = 0, y = 0, z = 1 }
		CCENGINE_API static const Vector3 Up;					//!< { x = 0, y = 1, z = 0 }
		CCENGINE_API static const Vector3 Right;				//!< { x = 1, y = 0, z = 0 }
		CCENGINE_API static const Vector3 Zero;					//!< { x = 0, y = 0, z = 0 }
		CCENGINE_API static const Vector3 One;					//!< { x = 1, y = 1, z = 1 }
	};
}

#include "vector3.inl"