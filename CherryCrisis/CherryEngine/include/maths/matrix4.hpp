#pragma once

#include "vector4.hpp"
#include "maths_utils.hpp"

namespace CCMaths
{
	struct Matrix4
	{
		union
		{
			float data[16];		//!< Pointer on the Matrix4's data
			Vector4 row[4];		//!< Array of the four row of the Matrix4

			struct
			{
				Vector3 right;		//!< Right vector of the Matrix4
				float m4;
				Vector3 up;			//!< Up vector of the Matrix4
				float m8;
				Vector3 back;		//!< Back vector of the Matrix4
				float m12;
				Vector3 position;	//!< Position vector of the Matrix4
				float uniformScale;  //!< Uniform scale of the Matrix4
			};
		};

		/*!
		*	```
		*	{
		*		1, 0, 0, 0,
		*		0, 1, 0, 0,
		*		0, 0, 1, 0,
		*		0, 0, 0, 1
		*	}
		*	```
		*/
		CCENGINE_API static const Matrix4 Identity;

		//! Mutliply the Matrix4 by the Vector4
		/*!
		*	\param object: Matrix4 on the left side of the operator
		*	\param rhs: Vector4 on the right side of the operator
		*
		*	Create a Vector4 \b out equals to the product of \b object by \b rhs
		*
		*	\return Vector4 \b out
		*/
		inline Vector4 operator*(const Vector4& rhs) const;

		//! Mutliply the two Matrices
		/*!
		*	\param object: Matrix4 on the left side of the operator
		*	\param rhs: Matrix4 on the right side of the operator
		*
		*	Create a Matrix4 \b out equals to the product of \b object by \b rhs
		*
		*	\return Matrix4 \b out
		* 
		*	\sa operator*=(const Matrix4&)
		*	\sa operator*(const float) const
		*	\sa operator*=(const float)
		*/
		inline Matrix4 operator*(const Matrix4& rhs) const;

		//! Mutliply the two Matrices
		/*!
		*	\param object: Matrix4 on the left side of the operator
		*	\param rhs: Matrix4 on the right side of the operator
		*
		*	Mutliply \b object by \b rhs
		*
		*	\return Reference to \b object
		* 
		*	\sa operator*(const Matrix4&) const
		*	\sa operator*=(const float)
		*	\sa operator*(const float) const
		*/
		inline Matrix4& operator*=(const Matrix4& rhs);

		//! Mutliply the Matrix4 by the scalar
		/*!
		*	\param object: Matrix4 on the left side of the operator
		*	\param rhs: float on the right side of the operator
		*
		*	Create a Matrix4 \b out equals to the product of \b object by \b rhs
		*
		*	\return Matrix4 \b out
		* 
		*	\sa operator*=(const float)
		*	\sa operator*(const Matrix4&) const
		*	\sa operator*=(const Matrix4&)
		*/
		inline Matrix4 operator*(const float rhs) const;
		
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
		*	\sa operator*=(const Matrix4&)
		*	\sa operator*(const Matrix4&) const
		*/
		inline Matrix4& operator*=(const float rhs);

		//! Transpose the Matrix4
		/*!
		*	\attention Use as a member method of a specific object
		*
		*	\param object: Matrix4 from which the method is called
		*
		*	Create a new Matrix4 \b out as the transposed matrix of \b object
		*
		*	\return Matrix4 \b out
		*
		*	\note Has the same result as the non-object specific static version: Transpose(const Matrix4&)
		*
		*	\sa Transpose(const Matrix4&)
		*/
		inline Matrix4 Transpose() const;

		//! Transpose the Matrix4
		/*!
		*	\attention Use as an independant method of the struct (not relative to a specific object)
		*
		*	\param lhs: Matrix4 given as parameter
		*
		*	Create a new Matrix4 \b out as the transposed matrix of \b object
		*
		*	\return Matrix4 \b out
		*
		*	\note Has the same result as the object specific version: Transpose() const
		*
		*	\sa Transpose() const
		*/
		inline static Matrix4 Transpose(const Matrix4& lhs);

		//! Inverse the Matrix4
		/*!
		*	\attention Use as a member method of a specific object
		*
		*	\param object: Matrix4 from which the method is called
		*
		*	Create a new Matrix4 \b out as the inversed matrix of \b object
		*
		*	\return Matrix4 \b out
		*
		*	\note Has the same result as the non-object specific static version: Inverse(const Matrix4&)
		*
		*	\sa Inverse(const Matrix4&)
		*/
		inline Matrix4 Inverse() const;

		//! Inverse the Matrix4
		/*!
		*	\attention Used as an independant method of the struct (not relative to a specific object)
		*
		*	\param lhs: Matrix4 given as parameter
		*
		*	Create a new Matrix4 \b out as the inversed matrix of \b object
		*
		*	\return Matrix4 \b out
		*
		*	\note Has the same result as the object specific version: Inverse() const
		*
		*	\sa Inverse() const
		*/
		inline static Matrix4 Inverse(const Matrix4& lhs);

		//! Translation Matrix
		/*!
		*	\param in: Vector3 of the translation (position)
		*
		*	Create a new Matrix4 \b out with the translation
		*
		*	\return Matrix4 \b out
		*	```
		*	{
		*		1.f,  0.f,  0.f,  0.f,
		*		0.f,  1.f,  0.f,  0.f,
		*		0.f,  0.f,  1.f,  0.f,
		*		in.x, in.y, in.z, 1.f,
		*	}	
		*	```
		*/
		inline static Matrix4 Translate(const Vector3& inPosition);

		//! Scale Matrix
		/*!
		*	\param in: float of the scale
		*
		*	Create a new Matrix4 \b out with the scale
		*
		*	\return Matrix4 \b out
		*	```
		*	{
		*		in,  0.f, 0.f, 0.f,
		*		0.f, in,  0.f, 0.f,
		*		0.f, 0.f, in,  0.f,
		*		0.f, 0.f, 0.f, 1.f
		*	}
		*	```
		*
		*	\sa Scale(const Vector3&)
		*/
		inline static Matrix4 Scale(const float inScale);
		
		//! Scale Matrix
		/*!
		*	\param in: Vector3 of the scale
		*
		*	Create a new Matrix4 \b out with the scale
		*
		*	\return Matrix4 \b out
		*	```
		*	{
		*		in.x, 0.f,  0.f,  0.f,
		*		0.f,  in.y, 0.f,  0.f,
		*		0.f,  0.f,  in.z, 0.f,
		*		0.f,  0.f,  0.f,  1.f
		*	}
		*	```
		*
		*	\sa Scale(const float)
		*/
		inline static Matrix4 Scale(const Vector3& inScale);

		inline void NormalizeScale();

		inline Matrix4 NormalizedScale() const;

		//! Rotation Matrix
		/*!
		*	\param in: Vector3 euler angle in radians of the rotation { pitch, yaw, roll }
		*
		*	Create a new Matrix4 \b out with the product of the rotation on the 3 axis
		*
		*	\return Matrix4 \b out
		*
		*	\sa RotateX(const float)
		*	\sa RotateY(const float)
		*	\sa RotateZ(const float)
		*/
		inline static Matrix4 RotateXYZ(const Vector3& eulerAngles);

		//! Rotation Matrix
		/*!
		*	\param in: Vector3 euler angle in radians of the rotation { pitch, yaw, roll }
		*
		*	Create a new Matrix4 \b out with the product of the rotation on the 3 axis
		*
		*	\return Matrix4 \b out
		*
		*	\sa RotateX(const float)
		*	\sa RotateY(const float)
		*	\sa RotateZ(const float)
		*/
		inline static Matrix4 RotateZYX(const Vector3& eulerAngles);

		//! Rotation Matrix
		/*!
		*	\param in: Vector3 euler angle in radians of the rotation { pitch, yaw, roll }
		*
		*	Create a new Matrix4 \b out with the product of the rotation on the 3 axis
		*
		*	\return Matrix4 \b out
		*
		*	\sa RotateX(const float)
		*	\sa RotateY(const float)
		*	\sa RotateZ(const float)
		*/
		inline static Matrix4 RotateZXY(const Vector3& eulerAngles);

		//! Rotation Matrix
		/*!
		*	\param in: Vector3 euler angle in radians of the rotation { pitch, yaw, roll }
		*
		*	Create a new Matrix4 \b out with the product of the rotation on the 3 axis
		*
		*	\return Matrix4 \b out
		*
		*	\sa RotateX(const float)
		*	\sa RotateY(const float)
		*	\sa RotateZ(const float)
		*/
		inline static Matrix4 RotateYXZ(const Vector3& eulerAngles);

		//! Rotation Matrix
		/*!
		*	\param in: Vector3 euler angle in radians of the rotation { pitch, yaw, roll }
		*
		*	Create a new Matrix4 \b out with the product of the rotation on the 3 axis
		*
		*	\return Matrix4 \b out
		*
		*	\sa RotateX(const float)
		*	\sa RotateY(const float)
		*	\sa RotateZ(const float)
		*/
		inline static Matrix4 RotateXZY(const Vector3& eulerAngles);

		//! Rotation Matrix
		/*!
		*	\param in: Vector3 euler angle in radians of the rotation { pitch, yaw, roll }
		*
		*	Create a new Matrix4 \b out with the product of the rotation on the 3 axis
		*
		*	\return Matrix4 \b out
		*
		*	\sa RotateX(const float)
		*	\sa RotateY(const float)
		*	\sa RotateZ(const float)
		*/
		inline static Matrix4 RotateYZX(const Vector3& eulerAngles);
 
		//! X-axis Rotation Matrix
		/*!
		*	\param rad: float angle of the rotation in radians (pitch)
		*
		*	Create a new Matrix4 \b out with the rotation around X-axis
		* 
		*	With the \b cos and \b sin of the angle
		*
		*	\sa RotateX(const float, const float)
		*	\sa RotateZXY(const Vector3&)
		*	\sa RotateY(const float) ; RotateZ(const float)
		*/
		inline static Matrix4 RotateX(const float rad);

		//! Y-axis Rotation Matrix
		/*!
		*	\param rad: float angle of the rotation in radians (yaw)
		*
		*	Create a new Matrix4 \b out with the rotation around Y-axis
		* 
		*	With the \b cos and \b sin of the angle
		*
		*	\return Matrix4 \b out
		* 
		*	\sa	RotateY(const float, const float)
		*	\sa RotateZXY(const Vector3&)
		*	\sa RotateX(const float) ; RotateZ(const float)
		*/
		inline static Matrix4 RotateY(const float rad);

		//! Z-axis Rotation Matrix
		/*!
		*	\param rad: float angle of the rotation in radians (roll)
		*
		*	Create a new Matrix4 \b out with the rotation around Z-axis
		* 
		*	With the \b cos and \b sin of the angle
		*
		*	\return Matrix4 \b out
		* 
		*	\sa RotateZ(const float, const float)
		*	\sa RotateZXY(const Vector3&)
		*	\sa RotateX(const float) ; RotateY(const float)
		*/
		inline static Matrix4 RotateZ(const float rad);

		//! X-axis Rotation Matrix
		/*!
		*	\param cos: float cos of the angle of the rotation in radians (pitch)
		*	\param sin: float sin of the angle of the rotation in radians (pitch)
		*
		*	Create a new Matrix4 \b out with the rotation around X-axis
		*
		*	With the \b cos and \b sin of the angle
		*
		*	\return Matrix4 \b out
		*	```
		*	{
		*		1.f, 0.f, 0.f, 0.f,
		*		0.f, cos, sin, 0.f,
		*		0.f,-sin, cos, 0.f,
		*		0.f, 0.f, 0.f, 1.f,
		*	}
		*	```
		*
		*	\sa RotateX(const float)
		*	\sa RotateZXY(const Vector3&)
		*	\sa RotateY(const float, const float) ; RotateZ(const float, const float)
		*/
		inline static Matrix4 RotateX(const float cos, const float sin);

		//! Y-axis Rotation Matrix
		/*!
		*	\param cos: float cos of the angle of the rotation in radians (yaw)
		*	\param sin: float sin of the angle of the rotation in radians (yaw)
		*
		*	Create a new Matrix4 \b out with the rotation around Y-axis
		*
		*	With the \b cos and \b sin of the angle
		*
		*	\return Matrix4 \b out
		*	```
		*	{
		*		cos, 0.f,-sin, 0.f,
		*		0.f, 1.f, 0.f, 0.f,
		*		sin, 0.f, cos, 0.f,
		*		0.f, 0.f, 0.f, 1.f,
		*	}
		*	```
		*
		*	\sa RotateY(const float)
		*	\sa RotateZXY(const Vector3&)
		*	\sa RotateX(const float const float) ; RotateZ(const float, const float)
		*/
		inline static Matrix4 RotateY(const float cos, const float sin);

		//! Z-axis Rotation Matrix
		/*!
		*	\param cos: float cos of the angle of the rotation in radians (roll)
		*	\param sin: float sin of the angle of the rotation in radians (roll)
		*
		*	Create a new Matrix4 \b out with the rotation around Z-axis
		*
		*	With the \b cos and \b sin of the angle
		*
		*	\return Matrix4 \b out
		*	```
		*	{
		*		cos, sin, 0.f, 0.f,
		*	   -sin, cos, 0.f, 0.f,
		*		0.f, 0.f, 1.f, 0.f,
		*		0.f, 0.f, 0.f, 1.f,
		*	}
		*	```
		*
		*	\sa RotateZ(const float)
		*	\sa RotateZXY(const Vector3&)
		*	\sa RotateX(const float, const float) ; RotateZ(const float, const float)
		*/
		inline static Matrix4 RotateZ(const float cos, const float sin);

		//! Decompose the Transform Matrix
		/*!
		*	\param matrix:	 Matrix4 to decompose
		*	\param outPos:	 Ref of Vector3 of the position in the matrix
		*	\param outRot:	 Ref of Vector3 of the euler angles in the matrix
		*	\param outScale: Ref of Vector3 of the scale in the matrix
		* 
		*	Decompose the Matrix4 into a scale vector, an euler angles vector and a translation/position vector
		*	The 3 vectors' ref are modified with those values
		*/
		inline static void Decompose(const Matrix4& matrix, Vector3& outPos, Vector3& outRot, Vector3& outScale);

		inline Matrix4 GetRotationMatrix() const;
		inline static Matrix4 GetRotationMatrix(const Matrix4& matrix);

		inline Matrix4 GetTranslationMatrix() const;
		inline static Matrix4 GetTranslationMatrix(const Matrix4& matrix);

		inline Matrix4 GetScaleMatrix() const;
		inline static Matrix4 GetScaleMatrix(const Matrix4& matrix);


		//! \hiderefs
		inline static Matrix4 Frustum(const float Left, const float Right, const float Bottom, const float Top, const float Near, const float Far);

		//! \hiderefs
		inline static Matrix4 Perspective(const float FovY, const float Aspect, const float Near, const float Far);

		//! \hiderefs
		inline static Matrix4 Orthographic(const float Left, const float Right, const float Bottom, const float Top, const float Near, const float Far);
		
		//! \hiderefs
		inline static Matrix4 ObliqueProjection(const Vector4& clipPlane, const Matrix4& projection);

		//! Look at Matrix4
		/*!
		*	\param Eye: Origin of the look at = position of the \b object
		*	\param At:	Target of the look at = position of the \b target
		*	\param Up:	Vector3 Up of the \b object
		* 
		*	Create a new Matrix4 \b out corresponding to the \b object looking at the \b target
		* 
		*	\return Matrix4 \b out
		*/
		inline static Matrix4 LookAt(const Vector3& Eye, const Vector3& At = { 0.f, 0.f, 0.f }, const Vector3& Up = { 0.f, 1.f, 0.f });
	};
}

#include "matrix4.inl"