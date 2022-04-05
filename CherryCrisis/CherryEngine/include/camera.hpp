#pragma once

#include "maths.hpp"

struct Camera
{
	enum Type
	{
		PERSPECTIVE,
		ORTHOGRAPHIC
	};

	Type type = PERSPECTIVE;

	float aspect = 4.f / 3.f;
	float near = 0.01f;
	float far = 200.f;
	float fovY = CCMaths::PI / 3.f;

	CCMaths::Vector3 position;
	CCMaths::Vector3 rotation;
};