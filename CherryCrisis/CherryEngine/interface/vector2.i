%{
	#include "maths/vector2.hpp"
%}

WRAP_VECTOR(CCMaths::Vector2)

%include maths/vector2.hpp

%extend CCMaths::Vector2
{
	%proxycode %{
		public override string ToString() => System.String.Format("Vector2({0}, {1})", x, y);
	%}
};

