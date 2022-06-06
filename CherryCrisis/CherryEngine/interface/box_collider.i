%{
	#include "box_collider.hpp"
	#include "collider.hpp"
%}

%include collider.i

%nodefaultctor BoxCollider;

class BoxCollider : public Collider
{
public:
	BoxCollider();

	%ignore BoxCollider();

	void SetScale(const CCMaths::Vector3& scale);
	CCMaths::Vector3 GetScale() { return m_editableScale; }
};