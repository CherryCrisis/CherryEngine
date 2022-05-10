%{
	#include "capsule_collider.hpp"
	#include "collider.hpp"
%}

%include collider.i

class CapsuleCollider : public Collider
{
public:
	CapsuleCollider();

	%ignore CapsuleCollider();

	void SetScale(const float& scale);
	float GetScale();

	void SetRadius(const float& radius);
	float GetRadius();
};