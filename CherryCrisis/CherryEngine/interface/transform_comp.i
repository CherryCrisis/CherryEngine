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
	void SetPosition(CCMaths::Vector3& position);
	void SetGlobalPosition(CCMaths::Vector3& position);
	CCMaths::Vector3 GetPosition();
	CCMaths::Vector3 GetGlobalPosition();

	void SetRotation(CCMaths::Vector3& rotation);
	void SetGlobalRotation(CCMaths::Vector3& rotation);
	CCMaths::Vector3 GetRotation();
	CCMaths::Vector3 GetGlobalRotation();

	void SetScale(CCMaths::Vector3& scale);
	void SetGlobalScale(CCMaths::Vector3& scale);
	CCMaths::Vector3 GetScale();
	CCMaths::Vector3 GetGlobalScale();

	CCMaths::Vector3 Up();
	CCMaths::Vector3 Right();
	CCMaths::Vector3 Forward();

	%ignore Transform();

	%proxycode
	%{
		public Vector3 position { get => GetPosition(); set => SetPosition(value); }
		public Vector3 eulerAngles { get => GetRotation(); set => SetRotation(value); }
		public Vector3 scale { get => GetScale(); set => SetScale(value); }
	%}
};