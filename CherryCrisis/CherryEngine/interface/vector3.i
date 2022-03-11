%{
	#include "maths/vector3.hpp"
%}

WRAP_VECTOR(CCMaths::Vector3)

%include vector2.i
%include maths/vector3.hpp

%extend CCMaths::Vector3
{
	%proxycode
	%{
		public override string ToString() => $"Vector3({x}, {y}, {z})";
	%}

};