%{
	#include "sphere_collider.hpp"
	#include "collider.hpp"
%}

%include collider.i

%nodefaultctor SphereCollider;

class SphereCollider : public Collider
{
public:
	SphereCollider();

	%ignore SphereCollider();

	void SetScale(const float& scale);
	float GetScale();
};