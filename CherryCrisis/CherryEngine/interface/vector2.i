%{
	#include "maths/vector2.hpp"
%}

WRAP_VECTOR(CCMaths::Vector2)

%include maths/vector2.hpp

%extend CCMaths::Vector2
{
	%proxycode %{
		public override string ToString() => $"Vector2({x}, {y})";
	%}
};

