#pragma once

#include "cherry_macros.hpp"

#include <PxPhysicsAPI.h>

#include "behaviour.hpp"

#include <set>


class Cell;

struct FilterGroup
{
	enum Enum
	{
		eACTOR = (1 << 0),
	};
};

namespace PhysicSystem
{
	class PhysicActor;

	class PhysicScene : public physx::PxSimulationEventCallback
	{
	private:
		physx::PxScene* m_pxScene = nullptr;
		physx::PxController* m_playerPxController = nullptr;

		bool	m_paused = false;
		float	m_gravity = 9.81f;

		std::vector<PhysicActor*> m_actors;

	public:
		Cell*	m_cell = nullptr;

		void	Update(float deltaTime);

		void	CreatePxScene();
		void	DestroyPxScene();

		void	AddCell(Cell* cell);

		void	AddActor(PhysicActor* actor);
		void	AddPxActor(PhysicActor* actor);
		bool	RemoveActor(PhysicActor* actor);
		void	RemovePxActor(PhysicActor* actor);
		int		PossessActor(PhysicActor* actor);

		// Implements PxSimulationEventCallback		
		virtual void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs);
		virtual void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count);
		virtual void onConstraintBreak(physx::PxConstraintInfo*, physx::PxU32) {}
		virtual void onWake(physx::PxActor**, physx::PxU32) {}
		virtual void onSleep(physx::PxActor**, physx::PxU32) {}
		virtual void onAdvance(const physx::PxRigidBody* const*, const physx::PxTransform*, const physx::PxU32) {}
		void	MoveCharacterController(float deltaTime);

		void			Pause(bool value)	{ m_paused = value; }
		bool			IsPaused()			{ return m_paused; }
		physx::PxScene* Get()				{ return m_pxScene; }
	};
}