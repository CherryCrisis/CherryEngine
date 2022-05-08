%{
	#include "box_collider.hpp"
	#include "collider.hpp"
%}

%include collider.i

class BoxCollider : public Collider
{
public:
	BoxCollider();

	%ignore BoxCollider();

	void SetScale(const float& scale);
	float GetScale();
};