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

	void SetRotation(const CCMaths::Vector3& rotation);
	void SetRotation(const CCMaths::Quaternion& rotation);
	void SetGlobalRotation(const CCMaths::Vector3& rotation);
	void SetGlobalRotation(const CCMaths::Quaternion& rotation);
	CCMaths::Vector3 GetEuler();
	CCMaths::Quaternion GetRotation();
	CCMaths::Quaternion GetGlobalRotation();

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
		public Vector3 eulerAngles { get => GetEuler(); set => SetRotation(value); }
		public Quaternion rotation { get => GetRotation(); set => SetRotation(value); }
		public Vector3 scale { get => GetScale(); set => SetScale(value); }
	%}
};