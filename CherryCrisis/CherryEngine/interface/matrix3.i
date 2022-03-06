%{
	#include "maths/matrix3.hpp"
%}

%include maths/matrix3.hpp

%extend CCMaths::Matrix3
{
	%proxycode
	%{
		public override string ToString()
		{
			throw NotImplementedException;
			//return System.String.Format("Matrix3({0}, {1}, {2},\n {3}, {4}, {5},\n {6}, {7}, {8},\n)", x, y, z, w);
		}
	%}

};