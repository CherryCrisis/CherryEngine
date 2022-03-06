%{
	#include "maths/vector4.hpp"
%}

WRAP_VECTOR(CCMaths::Vector4)

%include vector2.i
%include vector3.i
%include maths/vector4.hpp

%extend CCMaths::Vector4
{
	%proxycode
	%{
		public override string ToString() => System.String.Format("Vector4({0}, {1}, {2}, {3})", x, y, z, w);
	%}

};