%include "enums.swg"

%{
	#include "singleton.hpp"
	#include "physic_manager.hpp"

	using namespace PhysicSystem;
%}

%include maths.i

%include cell.i
%include physic_actor.i

%include std_string.i

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
	public PhysicActor* GetActor(Entity& owningEntity)
            => FindActor(Entity& owningEntity);
	%}

	static RaycastHit Raycast(Cell& scene, const CCMaths::Vector3& origin, const CCMaths::Vector3& dir, const float maxRange);
	static void AddForce(Entity* entity, const CCMaths::Vector3& force, EForceMode mode);
};