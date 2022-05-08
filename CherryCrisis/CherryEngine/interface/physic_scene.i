%{
	#include "physic_scene.hpp"
%}

%include physic_actor.i

class PhysicScene
{
		int				PossessActor(PhysicActor* actor);
		PhysicActor*	GetActor(int index);
}