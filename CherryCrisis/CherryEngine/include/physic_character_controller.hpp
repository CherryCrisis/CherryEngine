#pragma once

#include "cherry_macros.hpp"

namespace physx
{
	class PxMaterial;
}

namespace CCMaths
{
	struct Vector3;
}

namespace PhysicSystem
{
	// CharacterController for a cell

	// TODO: Verify if it is needed to have on for each cell or just one in the cell manager (if they're all going to be the same)


	class CCENGINE_API PhysicCharacterController
	{
	public:
		CCMaths::Vector3 position = {};

		float radius		= 0.5f;
		float height		= 1.0f;
		float slopeLimit	= 0.707f;
		float maxJumpHeight = 0.0f;
		float contactOffset = 0.1f;
		float stepOffset	= 0.5f;
		float density		= 10.f;
		
		physx::PxMaterial* material = nullptr;

		PhysicCharacterController() {}

		// TODO: Serialization
	};
}