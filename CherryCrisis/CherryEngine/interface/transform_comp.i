%{
	#include "transform.hpp"
	#include "behaviour.hpp"
	#include "maths.hpp"
%}

%include behaviour.i
%include maths.i

class Transform : public Behaviour
{
private:
	void SetDirty();

public:
	void SetPosition(const CCMaths::Vector3& position);
	CCMaths::Vector3 GetPosition();

	void SetRotation(const CCMaths::Vector3& rotation);
	CCMaths::Vector3 GetRotation();

	void SetScale(const CCMaths::Vector3& scale);
	CCMaths::Vector3 GetScale();

	%ignore Transform();

	%proxycode
	%{
		public Vector3 position { get => GetPosition(); set => SetPosition(value); }
		public Vector3 eulerAngles { get => GetRotation(); set => SetRotation(value); }
		public Vector3 scale { get => GetScale(); set => SetScale(value); }
	%}
};