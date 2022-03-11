#pragma once

#include "pch.hpp"

namespace CCMaths
{
	const Vector2 Vector2::XAxis	= { 1.f, 0.f };
	const Vector2 Vector2::YAxis	= { 0.f, 1.f };
	const Vector2 Vector2::Zero		= { 0.f, 0.f };
	const Vector2 Vector2::One		= { 1.f, 1.f };

	const Vector3 Vector3::XAxis	= { 1.f, 0.f, 0.f };
	const Vector3 Vector3::YAxis	= { 0.f, 1.f, 0.f };
	const Vector3 Vector3::ZAxis	= { 0.f, 0.f, 1.f };
	const Vector3 Vector3::Forward	= Vector3::ZAxis;
	const Vector3 Vector3::Up		= Vector3::YAxis;
	const Vector3 Vector3::Right	= Vector3::XAxis;
	const Vector3 Vector3::Zero		= { 0.f };
	const Vector3 Vector3::One		= { 1.f };

	const Vector4 Vector4::XAxis	= { 1.f, 0.f, 0.f, 0.f };
	const Vector4 Vector4::YAxis	= { 0.f, 1.f, 0.f, 0.f };
	const Vector4 Vector4::ZAxis	= { 0.f, 0.f, 1.f, 0.f };
	const Vector4 Vector4::WAxis	= { 0.f, 0.f, 0.f, 1.f };
	const Vector4 Vector4::Zero		= { 0.f };
	const Vector4 Vector4::One		= { 1.f };

	const Matrix4 Matrix4::Identity =
	{
		1.f, 0.f, 0.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f,
	};
}