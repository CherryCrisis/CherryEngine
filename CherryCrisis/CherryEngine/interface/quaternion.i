%{
	#include "maths/quaternion.hpp"
%}

%ignore CCMaths::Quaternion::operator +=;
%ignore CCMaths::Quaternion::operator *=;
%ignore CCMaths::Quaternion::operator -=;
%ignore CCMaths::Quaternion::operator /=;
%rename(operator_add) CCMaths::Quaternion::operator +;
%rename(operator_substract) CCMaths::Quaternion::operator -;
%rename(operator_multiply) CCMaths::Quaternion::operator *;
%rename(operator_divide) CCMaths::Quaternion::operator /;


%include maths/quaternion.hpp

%extend CCMaths::Quaternion
{
	%proxycode %{
		public override string ToString() => $"Quaternion({x}, {y}, {z}, {w})";

		public static Quaternion operator +(Quaternion lhs, Quaternion rhs) => lhs.operator_add(rhs);
		public static Quaternion operator -(Quaternion lhs, Quaternion rhs) => lhs.operator_substract(rhs);
		public static Quaternion operator *(Quaternion lhs, Quaternion rhs) => lhs.operator_multiply(rhs);
		public static Vector3 operator *(Quaternion lhs, Vector3 rhs) => lhs.operator_multiply(rhs);
		public static Quaternion operator /(Quaternion lhs, Quaternion rhs) => lhs.operator_divide(rhs);
	%}
};
