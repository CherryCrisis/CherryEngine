#pragma once

namespace CCMaths
{
	struct Vector2
	{
		union
		{
			struct { float x, y; };		//!< Struct of the four components of the Vector2

			float data[2];				//!< Pointer on the Vector2's data
		};

		const static Vector2 XAxis;		//!< { x = 1, y = 0 }
		const static Vector2 YAxis;		//!< { x = 0, y = 1 }
		const static Vector2 Zero;		//!< { x = 0, y = 0 }
		const static Vector2 One;		//!< { x = 1, y = 1 }
		
		//! Construct the Vector2 as the \b Zero Vector2
		Vector2()
			:x(0.f), y(0.f) {}

		//! Construct the Vector2 with the four components equal to the given value
		Vector2(const float value)
			:x(value), y(value) {}

		//! Construct the Vector2 with the four given members
		Vector2(const float x, const float y)
			: x(x), y(y) {}

		//! Add the two Vector2s
		/*!
		*	\param object: Vector2 on the left side of the operator
		*	\param rhs: Vector2 on the right side of the operator
		*
		*	Create a Vector2 \b out equals to the sum of \b object and \b rhs
		*
		*	\return Vector2 \b out
		*
		*	\sa operator+=(const Vector2&)
		*	\sa operator+(const float) const
		*	\sa operator+=(const float)
		*/
		inline Vector2 operator+(const Vector2& rhs) const;

		//! Add the two Vector2s
		/*!
		*	\param object: Vector2 on the left side of the operator
		*	\param rhs: Vector2 on the right side of the operator
		*
		*	Add \b rhs to \b object
		*
		*	\return Reference to \b object
		*
		*	\sa operator+(const Vector2&) const
		*	\sa operator+=(const float)
		*	\sa operator+(const float) const
		*/
		inline Vector2& operator+=(const Vector2& rhs);

		//! Add a scalar to the Vector2
		/*!
		*	\param object: Vector2 on the left side of the operator
		*	\param rhs: float on the right side of the operator
		*
		*	Create a Vector2 \b out by adding \b rhs and each member of \b object
		*
		*	\return Vector2 \b out
		*
		*	\sa operator+=(const float)
		*	\sa operator+(const Vector2&) const
		*	\sa operator+=(const Vector2&)
		*/
		inline Vector2 operator+(const float rhs) const;

		//! Add the two Vector2s
		/*!
		*	\param object: Vector2 on the left side of the operator
		*	\param rhs: Vector2 on the right side of the operator
		*
		*	Add \b rhs to each member of \b object
		*
		*	\return Reference to \b object
		*
		*	\sa operator+(const float) const
		*	\sa operator+=(const Vector2&)
		*	\sa operator+(const Vector2&) const
		*/
		inline Vector2& operator+=(const float rhs);

		//! Oppose the Vector2
		/*!
		*	\param object: Quaternion from which the method is called
		*
		*	Use as \b -object
		*	Create a new Vector2 \b out with each member equals to - the corresponding \b object's member
		*
		*	\return Vector2 \b out
		*/
		inline Vector2 operator-() const;

		//! Substract the two Vector2s
		/*!
		*	\param object: Vector2 on the left side of the operator
		*	\param rhs: Vector2 on the right side of the operator
		*
		*	Create a Vector2 \b out equals to the difference between \b object and \b rhs
		*
		*	\return Vector2 \b out
		*
		*	\sa operator-=(const Vector2&)
		*	\sa	operator-(const float) const
		*	\sa operator-=(const float)
		*/
		inline Vector2 operator-(const Vector2& rhs) const;

		//! Substract the two Vector2s 
		/*!
		*	\param object: Vector2 on the left side of the operator
		*	\param rhs: Vector2 on the right side of the operator
		*
		*	Substract \b rhs from \b object
		*
		*	\return Reference to \b object
		*
		*	\sa	operator-(const Vector2&) const
		*	\sa operator-=(const float)
		*	\sa operator-(const float) const
		*/
		inline Vector2& operator-=(const Vector2& rhs);

		//! Substract a scalar from the Vector2
		/*!
		*	\param object: Vector2 on the left side of the operator
		*	\param rhs: float on the right side of the operator
		*
		*	Create a Vector2 \b out equals to the difference between each member of \b object and \b rhs
		*
		*	\return Vector2 \b out
		*
		*	\sa operator-=(const float)
		*	\sa	operator-(const Vector2&) const
		*	\sa operator-=(const Vector2&)
		*/
		inline Vector2 operator-(const float rhs) const;

		//! Substract a scalar from the Vector2
		/*!
		*	\param object: Vector2 on the left side of the operator
		*	\param rhs: Vector2 on the right side of the operator
		*
		*	Substract \b rhs from each member of \b object
		*
		*	\return Reference to \b object
		*
		*	\sa operator-(const float) const
		*	\sa operator-=(const Vector2&)
		*	\sa	operator-(const Vector2&) const
		*/
		inline Vector2& operator-=(const float rhs);

		//! The dot product between two Vector2s
		/*!
		*	\attention Use as a member method of a specific object
		*
		*	\param object: Vector2 from which the method is called
		*	\param rhs: Vector2 given as parameter
		*
		*	Create a new float \b out equals to the dot product of \b object and \b rhs
		*
		*	\return float \b out
		*
		*	\sa Dot(const Vector2&) const
		*	\sa Dot(const Vector2&, const Vector2&)
		*/
		inline float operator*(const Vector2& rhs) const;

		//! Mutliply the Vector2 by the scalar
		/*!
		*	\param object: Vector2 on the left side of the operator
		*	\param rhs: float on the right side of the operator
		*
		*	Create a Vector2 \b out equals to the product of \b object by \b rhs
		*
		*	\return Vector2 \b out
		*
		*	\sa operator*=(const float)
		*/
		inline Vector2 operator*(const float rhs) const;

		//! Mutliply the Vector2 by the scalar
		/*!
		*	\param object : Vector2 on the left side of the operator
		*	\param rhs : float on the right side of the operator
		*
		*	Mutliply \b object by \b rhs
		*
		*	\return Reference to \b object
		*
		*	\sa operator*(const float) const
		*/
		inline Vector2& operator*=(const float rhs);

		//! Mutliply the Vector2 by the inverse of the scalar
		/*!
		*	\param object: Vector2 on the left side of the operator
		*	\param rhs: float on the right side of the operator
		*
		*	Create a Vector2 \b out equals to the division of \b object by \b rhs
		*
		*	\return Vector2 \b out
		*
		*	\sa operator/=(const float)
		*/
		inline Vector2 operator/(const float rhs) const;

		//! Mutliply the Vector2 by the inverse of the scalar
		/*!
		*	\param object : Vector2 on the left side of the operator
		*	\param rhs : float on the right side of the operator
		*
		*	Divide \b object by \b rhs
		*
		*	\return Reference to \b object
		*
		*	\sa operator/(const float) const
		*/
		inline Vector2& operator/=(const float rhs);

		//! Compare the two Vector2s
		/*!
		*	\param object : Vector2 on the left side of the operator
		*	\param rhs : Vector2 on the right side of the operator
		*
		*	Check equality of \b object and \b rhs member to member
		*
		*	\return \b True if all checks are true
		*
		*	\sa operator!=(const Vector2&) const
		*/
		inline bool operator==(const Vector2& rhs) const;

		//! Compare the two Vector2s
		/*!
		*	\param object : Vector2 on the left side of the operator
		*	\param rhs : Vector2 on the right side of the operator
		*
		*	Check equality of \b object and \b rhs member to member
		*
		*	\return \b True if at least one check is false
		*
		*	\sa operator==(const Vector2&) const
		*/
		inline bool operator!=(const Vector2& rhs) const;

		//! The dot product between two Vector2s
		/*!
		*	\attention Use as a member method of a specific object
		*
		*	\param object: Vector2 from which the method is called
		*	\param rhs: Vector2 given as parameter
		*
		*	Create a new float \b out equals to the dot product of \b object and \b rhs
		*
		*	\return float \b out
		*
		*	\note Has the same result as the non-object specific static version: Dot(const Vector2&, const Vector2&)
		*
		*	\sa Dot(const Vector2&, const Vector2&)
		*	\sa operator*(const Vector2&) const
		*/
		inline float Dot(const Vector2& rhs) const;

		//! The dot product between two Vector2s
		/*!
		*	\attention Use as an independant method of the struct (not relative to a specific object)
		*
		*	\param lhs: Vector2 given as parameter
		*	\param rhs: Vector2 given as parameter
		*
		*	Create a new float \b out equals to the dot product of \b object and \b rhs
		*
		*	\return float \b out
		*
		*	\note Has the same result as the object specific version: Dot(const Vector2&) const
		*
		*	\sa Dot(const Vector2&) const
		*	\sa operator*(const Vector2&) const
		*/
		inline static float Dot(const Vector2& lhs, const Vector2& rhs);

		//! Square Norm of the Vector2
		/*!
		*	\attention Use as a member method of a specific object
		*
		*	\param object: Vector2 from which the method is called
		*
		*	Create a new float \b out equals to the square norm of \b object
		*
		*	\return float \b out
		*
		*	\sa Length() const
		*/
		inline float SquareLength() const;

		//! Norm of the Vector2
		/*!
		*	\attention Use as a member method of a specific object
		*
		*	\param object: Vector2 from which the method is called
		*
		*	Create a new float \b out equals to the norm of \b object
		*
		*	\return float \b out
		*
		*	\sa SquareLength() const
		*/
		inline float Length() const;

		//! Normalize the Vector2
		/*!
		*	\attention Use as a member method of a specific object
		*
		*	\param object: Vector2 from which the method is called
		*
		*	Normalize \b object
		*
		*	\return Reference to \b object
		*
		*	\note Has the same result as the non-object specific static version: Normalize(Vector2&)
		*
		*	\note Use Normalized() if you \b don't want to modify \b object
		*
		*	\sa Normalize(Vector2&)
		*	\sa Normalized() const
		*	\sa Normalized(const Vector2&)
		*/
		inline Vector2& Normalize();

		//! Normalize the Vector2
		/*!
		*	\attention Use as an independant method of the struct (not relative to a specific object)
		*
		*	\param outVector: Vector2 given as parameter
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
		*	\sa Normalized(const Vector2&)
		*	\sa Normalized() const
		*/
		inline Vector2& Normalize(Vector2& outVector);

		//! Normalize the Vector2
		/*!
		*	\attention Use as a member method of a specific object
		*
		*	\param object: Vector2 from which the method is called
		*
		*	Create a new Vector2 \b out equals to normalized \b object
		*
		*	\return Vector2 \b out
		*
		*	\note Has the same result as the non-object specific static version: Normalized(const Vector2&)
		*
		*	\note Use Normalize() if you want to modify \b object
		*
		*	\sa Normalized(const Vector2&)
		*	\sa Normalize()
		*	\sa Normalize(Vector2&)
		*/
		inline Vector2 Normalized() const;

		//! Normalize the Vector2
		/*!
		*	\attention Use as an independant method of the struct (not relative to a specific object)
		*
		*	\param inVector: Vector2 given as parameter
		*
		*	Create a new Vector2 \b out equals to normalized \b inVector2
		*
		*	\return Vector2 \b out
		*
		*	\note Has the same result as the object specific version: Normalize() const
		*
		*	\note Use Normalize() if you want to modify \b inVector
		*
		*	\sa Normalized() const
		*	\sa Normalize(Vector2&)
		*	\sa Normalize()
		*/
		inline static Vector2 Normalized(const Vector2& inVector);
	};
}

#include "vector2.inl"