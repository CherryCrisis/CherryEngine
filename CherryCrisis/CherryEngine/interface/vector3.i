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
		public override string ToString() => System.String.Format("Vector3({0}, {1}, {2})", x, y, z);
	%}

};