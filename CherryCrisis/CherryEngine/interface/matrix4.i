%{
	#include "maths/matrix4.hpp"
%}

%ignore CCMaths::Matrix4::operator *=;
%rename(operator_multiply) CCMaths::Matrix4::operator *;

%include maths/matrix4.hpp

%extend CCMaths::Matrix4
{
	%proxycode %{
		public static Matrix4 operator *(Matrix4 lhs, Matrix4 rhs) => lhs.operator_multiply(rhs);
	%}
};
