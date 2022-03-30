#pragma once

#include "cherry_macros.hpp"

#include "behaviour.hpp"
#include "physic_manager.hpp"

namespace PhysicSystem
{
	class CCENGINE_API PhysicScene
	{
	private:
		physx::PxScene* m_pxScene = nullptr;

		bool	m_paused = false;
		float	m_gravity = 9.81f;

		// TODO: Add reference to cell;

	public:
		std::vector<PhysicActor*> actors;

		void	Update(float deltaTime);

		void	CreatePxScene();
		void	DestroyPxScene();

		void	AddActor(PhysicActor* actor);
		bool	RemoveActor(PhysicActor* actor);
		int		PossessActor(PhysicActor* actor);

		void			Pause(bool value)	{ m_paused = value; }
		bool			IsPaused()			{ return m_paused; }
		physx::PxScene* Get()				{ return m_pxScene; }
	};
}