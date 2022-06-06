%include "enums.swg"

%{
	#include "singleton.hpp"
	#include "physic_manager.hpp"
%}

%include maths.i

%include cell.i
%include entity.i
%include physic_actor.i

%include std_string.i

namespace PhysicSystem
{
	struct RaycastHit
	{
		CCMaths::Vector3	position;
		CCMaths::Vector3	normal;
		float				distance;

		PhysicActor* actor = nullptr;
	};

	enum class EForceMode
	{
		eFORCE,
		eIMPULSE,
		eVELOCITY_CHANGE,
		eACCELERATION
	};

	class PhysicManager
	{
	public:
		static PhysicManager* GetInstance();

		PhysicActor* FindActor(Entity& owningEntity);

		%proxycode %{
		public PhysicActor GetActor(Entity owningEntity) => FindActor(owningEntity);
		%}

		static RaycastHit Raycast(Cell& cell, const CCMaths::Vector3& origin, const CCMaths::Vector3& dir, const float maxRange);
		static void AddForce(Entity* entity, const CCMaths::Vector3& force, PhysicSystem::EForceMode mode);
	};
}
