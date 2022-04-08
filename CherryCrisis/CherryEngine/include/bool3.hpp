#pragma once

struct Bool3
{
	union
	{
		struct { bool x, y, z; };

		bool data[3];
	};

	//! Construct the Bool3 as the \b false Bool3
	Bool3()
		: x(false), y(false), z(false) {}

	//! Construct the Bool3 with the three components equal to the given value
	Bool3(const bool value)
		:x(value), y(value), z(value) {}

	//! Construct the Bool3 with the three given members
	Bool3(const bool x, const bool y, const bool z)
		: x(x), y(y), z(z) {}
};