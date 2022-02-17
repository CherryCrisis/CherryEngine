#pragma once

#include "vector3.hpp"

namespace CCMaths
{
	struct Matrix3
	{
		union
		{
			float data[9];
			Vector3 Row[3];
		};
	};
}