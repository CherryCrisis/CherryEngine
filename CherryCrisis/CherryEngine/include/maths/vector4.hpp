#pragma once

#include "vector3.hpp"
#include "maths_utils.hpp"

namespace CCMaths
{
	struct Vector4
	{
		union
		{
			struct { float x, y, z, w; };	//!< Struct of the four components of the Vector4
			struct { float r, g, b, a; };	//!< Struct of the four components of the Vector4

			float data[4];					//!< Pointer on the Vector4's data

			Vector3 xyz;
			Vector3 rgb;
		};

		//! Construct the Vector4 as the \b Zero Vector4
		Vector4()
			: x(0.f), y(0.f), z(0.f), w(0.f) {}

		//! Construct the Vector4 with the four components equal to the given value
		Vector4(const float value)
			: x(value), y(value), z(value), w(value) {}

		//! Construct the Vector4 with the four given members
		Vector4(float x, float y, float z, float w)
			: x(x), y(y), z(z), w(w) {}

		//! Construct the Vector4 from a Vector3 and a float
		Vector4(const Vector3& xyz, float w)
			: xyz(xyz)
		{
			this->w = w;
		}

		//! Add the two Vector4s
		/*!
		*	\param object: Vector4 on the left side of the operator
		*	\param rhs: Vector4 on the right side of the operator
		*
		*	Create a Vector4 \b out equals to the sum of \b object and \b rhs
		*
		*	\return Vector4 \b out
		* 
		*	\sa operator+=(const Vector4&)
		*	\sa operator+(const float) const
		*	\sa operator+=(const float)
		*/
		inline Vector4 operator+(const Vector4& rhs) const;

		//! Add the two Vector4s
		/*!
		*	\param object: Vector4 on the left side of the operator
		*	\param rhs: Vector4 on the right side of the operator
		*
		*	Add \b rhs to \b object
		*
		*	\return Reference to \b object
		* 
		*	\sa operator+(const Vector4&) const
		*	\sa operator+=(const float)
		*	\sa operator+(const float) const
		*/
		inline Vector4& operator+=(const Vector4& rhs);
		
		//! Add a scalar to the Vector4
		/*!
		*	\param object: Vector4 on the left side of the operator
		*	\param rhs: float on the right side of the operator
		*
		*	Create a Vector4 \b out by adding \b rhs and each member of \b object
		*
		*	\return Vector4 \b out
		* 
		*	\sa operator+=(const float)
		*	\sa operator+(const Vector4&) const
		*	\sa operator+=(const Vector4&)
		*/	
		inline Vector4 operator+(const float rhs) const;

		//! Add the two Vector4s
		/*!
		*	\param object: Vector4 on the left side of the operator
		*	\param rhs: Vector4 on the right side of the operator
		*
		*	Add \b rhs to each member of \b object
		*
		*	\return Reference to \b object
		* 
		*	\sa operator+(const float) const
		*	\sa operator+=(const Vector4&)
		*	\sa operator+(const Vector4&) const
		*/
		inline Vector4& operator+=(const float rhs);

		//! Oppose the Vector4
		/*!
		*	\param object: Quaternion from which the method is called
		*
		*	Use as \b -object
		*	Create a new Vector4 \b out with each member equals to - the corresponding \b object's member
		*
		*	\return Vector4 \b out
		*/
		inline Vector4 operator-() const;
		
		//! Substract the two Vector4s
		/*!
		*	\param object: Vector4 on the left side of the operator
		*	\param rhs: Vector4 on the right side of the operator
		*
		*	Create a Vector4 \b out equals to the difference between \b object and \b rhs
		*
		*	\return Vector4 \b out
		* 
		*	\sa operator-=(const Vector4&)
		*	\sa	operator-(const float) const
		*	\sa operator-=(const float)
		*/
		inline Vector4 operator-(const Vector4& rhs) const;

		//! Substract the two Vector4s 
		/*!
		*	\param object: Vector4 on the left side of the operator
		*	\param rhs: Vector4 on the right side of the operator
		*
		*	Substract \b rhs from \b object
		*
		*	\return Reference to \b object
		* 
		*	\sa	operator-(const Vector4&) const
		*	\sa operator-=(const float)
		*	\sa operator-(const float) const
		*/
		inline Vector4& operator-=(const Vector4& rhs);
		
		//! Substract a scalar from the Vector4
		/*!
		*	\param object: Vector4 on the left side of the operator
		*	\param rhs: float on the right side of the operator
		*
		*	Create a Vector4 \b out equals to the difference between each member of \b object and \b rhs
		*
		*	\return Vector4 \b out
		*
		*	\sa operator-=(const float)
		*	\sa	operator-(const Vector4&) const
		*	\sa operator-=(const Vector4&)
		*/
		inline Vector4 operator-(const float rhs) const;

		//! Substract a scalar from the Vector4
		/*!
		*	\param object: Vector4 on the left side of the operator
		*	\param rhs: Vector4 on the right side of the operator
		*
		*	Substract \b rhs from each member of \b object
		*
		*	\return Reference to \b object
		*
		*	\sa operator-(const float) const
		*	\sa operator-=(const Vector4&)
		*	\sa	operator-(const Vector4&) const
		*/
		inline Vector4& operator-=(const float rhs);

		//! The dot product between two Vector4s
		/*!
		*	\attention Use as a member method of a specific object
		*
		*	\param object: Vector4 from which the method is called
		*	\param rhs: Vector4 given as parameter
		*
		*	Create a new float \b out equals to the dot product of \b object and \b rhs
		*
		*	\return float \b out
		* 
		*	\sa Dot(const Vector4&) const
		*	\sa Dot(const Vector4&, const Vector4&)
		*/
		inline float operator*(const Vector4& rhs) const;

		//! Mutliply the Vector4 by the scalar
		/*!
		*	\param object: Vector4 on the left side of the operator
		*	\param rhs: float on the right side of the operator
		*
		*	Create a Vector4 \b out equals to the product of \b object by \b rhs
		*
		*	\return Vector4 \b out
		* 
		*	\sa operator*=(const float)
		*/
		inline Vector4 operator*(const float rhs) const;

		//! Mutliply the Vector4 by the scalar
		/*!
		*	\param object : Vector4 on the left side of the operator
		*	\param rhs : float on the right side of the operator
		*
		*	Mutliply \b object by \b rhs
		*
		*	\return Reference to \b object
		* 
		*	\sa operator*(const float) const
		*/
		inline Vector4& operator*=(const float rhs);

		//! Mutliply the Vector4 by the inverse of the scalar
		/*!
		*	\param object: Vector4 on the left side of the operator
		*	\param rhs: float on the right side of the operator
		*
		*	Create a Vector4 \b out equals to the division of \b object by \b rhs
		*
		*	\return Vector4 \b out
		* 
		*	\sa operator/=(const float)
		*/
		inline Vector4 operator/(const float rhs) const;

		//! Mutliply the Vector4 by the inverse of the scalar
		/*!
		*	\param object : Vector4 on the left side of the operator
		*	\param rhs : float on the right side of the operator
		*
		*	Divide \b object by \b rhs
		*
		*	\return Reference to \b object
		*
		*	\sa operator/(const float) const
		*/
		inline Vector4& operator/=(const float rhs);

		//! Compare the two Vector4s
		/*!
		*	\param object : Vector4 on the left side of the operator
		*	\param rhs : Vector4 on the right side of the operator
		*
		*	Check equality of \b object and \b rhs member to member
		*
		*	\return \b True if all checks are true
		* 
		*	\sa operator!=(const Vector4&) const
		*/
		inline bool operator==(const Vector4& rhs) const;

		//! Compare the two Vector4s
		/*!
		*	\param object : Vector4 on the left side of the operator
		*	\param rhs : Vector4 on the right side of the operator
		*
		*	Check inequality of \b object and \b rhs member to member
		*
		*	\return \b True if at least one check is false
		* 
		*	\sa operator==(const Vector4&) const
		*/
		inline bool operator!=(const Vector4& rhs) const;

		//! Compare the two Vector4s
		/*!
		*	\param object : Vector4 on the left side of the operator
		*	\param rhs : Vector4 on the right side of the operator
		*
		*	Check inferiority of \b object and \b rhs member to member
		*
		*	\return \b True if at least one check is false
		*
		*	\sa operator<(const Vector4&) const
		*/
		inline bool operator<=(const Vector4& rhs) const;

		//! Compare the two Vector4s
		/*!
		*	\param object : Vector4 on the left side of the operator
		*	\param rhs : Vector4 on the right side of the operator
		*
		*	Check strict inferiority of \b object and \b rhs member to member
		*
		*	\return \b True if at least one check is false
		*
		*	\sa operator<=(const Vector4&) const
		*/
		inline bool operator<(const Vector4& rhs) const;

		//! Compare the two Vector4s
		/*!
		*	\param object : Vector4 on the left side of the operator
		*	\param rhs : Vector4 on the right side of the operator
		*
		*	Check superiority of \b object and \b rhs member to member
		*
		*	\return \b True if at least one check is false
		*
		*	\sa operator>(const Vector4&) const
		*/
		inline bool operator>=(const Vector4& rhs) const;

		//! Compare the two Vector4s
		/*!
		*	\param object : Vector4 on the left side of the operator
		*	\param rhs : Vector4 on the right side of the operator
		*
		*	Check strict superiority of \b object and \b rhs member to member
		*
		*	\return \b True if at least one check is false
		*
		*	\sa operator>=(const Vector4&) const
		*/
		inline bool operator>(const Vector4& rhs) const;

		//! The dot product between two Vector4s
		/*! 
		*	\attention Use as a member method of a specific object
		*
		*	\param object: Vector4 from which the method is called
		*	\param rhs: Vector4 given as parameter
		*
		*	Create a new float \b out equals to the dot product of \b object and \b rhs
		*
		*	\return float \b out
		*
		*	\note Has the same result as the non-object specific static version: Dot(const Vector4&, const Vector4&)
		* 
		*	\sa Dot(const Vector4&, const Vector4&)
		*	\sa operator*(const Vector4&) const
		*/
		inline float Dot(const Vector4& rhs) const;

		//! The dot product between two Vector4s
		/*!
		*	\attention Use as an independant method of the struct (not relative to a specific object)
		*
		*	\param lhs: Vector4 given as parameter
		*	\param rhs: Vector4 given as parameter
		*
		*	Create a new float \b out equals to the dot product of \b object and \b rhs
		*
		*	\return float \b out
		*
		*	\note Has the same result as the object specific version: Dot(const Vector4&) const
		* 
		*	\sa Dot(const Vector4&) const
		*	\sa operator*(const Vector4&) const
		*/
		inline static float Dot(const Vector4& lhs, const Vector4& rhs);

		//! Square Norm of the Vector4
		/*!
		*	\attention Use as a member method of a specific object
		*
		*	\param object: Vector4 from which the method is called
		*
		*	Create a new float \b out equals to the square norm of \b object
		*
		*	\return float \b out
		* 
		*	\sa Length() const
		*/
		inline float SquareLength() const;

		//! Norm of the Vector4
		/*!
		*	\attention Use as a member method of a specific object
		*
		*	\param object: Vector4 from which the method is called
		*
		*	Create a new float \b out equals to the norm of \b object
		*
		*	\return float \b out
		* 
		*	\sa SquareLength() const
		*/
		inline float Length() const;

		//! Normalize the Vector4
		/*!
		*	\attention Use as a member method of a specific object
		*
		*	\param object: Vector4 from which the method is called
		*
		*	Normalize \b object
		*
		*	\return Reference to \b object
		*
		*	\note Has the same result as the non-object specific static version: Normalize(Vector4&)
		* 
		*	\note Use Normalized() if you \b don't want to modify \b object
		* 
		*	\sa Normalize(Vector4&)
		*	\sa Normalized() const
		*	\sa Normalized(const Vector4&)
		*/
		inline Vector4& Normalize();

		//! Normalize the Vector4
		/*!
		*	\attention Use as an independant method of the struct (not relative to a specific object)
		*
		*	\param outVector4: Vector4 given as parameter
		*
		*	Normalize \b outVector4
		*
		*	\return Reference to \b outVector4
		*
		*	\note Has the same result as the object specific version: Normalize()
		* 
		*	\note Use Normalized() if you \b don't want to modify \b outVector4
		*
		*	\sa Normalize()
		*	\sa Normalized(const Vector4&)
		*	\sa Normalized() const
		*/
		inline static Vector4& Normalize(Vector4& outVector);

		//! Normalize the Vector4
		/*!
		*	\attention Use as a member method of a specific object
		*
		*	\param object: Vector4 from which the method is called
		*
		*	Create a new Vector4 \b out equals to normalized \b object
		*
		*	\return Vector4 \b out
		*
		*	\note Has the same result as the non-object specific static version: Normalized(const Vector4&)
		*
		*	\note Use Normalize() if you want to modify \b object
		* 		
		*	\sa Normalized(const Vector4&)
		*	\sa Normalize()
		*	\sa Normalize(Vector4&) 
		*/
		inline Vector4 Normalized() const;

		//! Normalize the Vector4
		/*!
		*	\attention Use as an independant method of the struct (not relative to a specific object)
		*
		*	\param inVector4: Vector4 given as parameter
		*
		*	Create a new Vector4 \b out equals to normalized \b inVector4
		*
		*	\return Vector4 \b out
		*
		*	\note Has the same result as the object specific version: Normalize() const
		*
		*	\note Use Normalize() if you want to modify \b inVector4
		* 
		*	\sa Normalized() const
		*	\sa Normalize(Vector4&)
		*	\sa Normalize()
		*/
		inline static Vector4 Normalized(const Vector4& inVector);

		static const Vector4 XAxis;			//!< { x = 1, y = 0, z = 0, w = 0 }
		static const Vector4 YAxis;			//!< { x = 0, y = 1, z = 0, w = 0 }
		static const Vector4 ZAxis;			//!< { x = 0, y = 0, z = 1, w = 0 }
		static const Vector4 WAxis;			//!< { x = 0, y = 0, z = 0, w = 1 }
		static const Vector4 Zero;			//!< { x = 0, y = 0, z = 0, w = 0 }
		static const Vector4 One;			//!< { x = 1, y = 1, z = 1, w = 1 }
	};
}

#include "vector4.inl"