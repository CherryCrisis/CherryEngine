#pragma once

#include "cherry_macros.hpp"

#include "behaviour.hpp"

namespace physx
{
	class PxScene;
	class PxController;
}

class Cell;

namespace PhysicSystem
{
	class PhysicActor;

	class CCENGINE_API PhysicScene
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
		void	RemoveCell(Cell* cell);

		void	AddActor(PhysicActor* actor);
		bool	RemoveActor(PhysicActor* actor);
		int		PossessActor(PhysicActor* actor);

		void	MoveCharacterController(float deltaTime);

		void			Pause(bool value)	{ m_paused = value; }
		bool			IsPaused()			{ return m_paused; }
		physx::PxScene* Get()				{ return m_pxScene; }
	};
}