%{
	#include "transform.hpp"
	#include "behaviour.hpp"
	#include "maths.hpp"
%}

%include behaviour.i
%include maths.i

%nodefaultctor Transform;

class Transform : public Behaviour
{
private:
	void SetDirty();

public:
	void SetPosition(CCMaths::Vector3& position);
	void SetGlobalPosition(CCMaths::Vector3& position);
	CCMaths::Vector3 GetPosition();
	CCMaths::Vector3 GetGlobalPosition();

	void SetEuler(const CCMaths::Vector3& rotation);
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

	void Translate(const CCMaths::Vector3& translation);
	void Rotate(const CCMaths::Vector3& eulerAngles);
	void Rotate(const CCMaths::Vector3& axis, float angle);

	CCMaths::Vector3 Up();
	CCMaths::Vector3 Right();
	CCMaths::Vector3 Forward();

	bool IsRoot();

	void SetParent(Transform* parent, bool reapplyPosition, bool reapplyRot = false, bool reapplyScale = false);
	void SetParent(Transform* parent);
	Transform* GetParent();
	Transform* GetRootParent();

	%proxycode
	%{
		public Vector3 position { get => GetPosition(); set => SetPosition(value); }
		public Vector3 eulerAngles { get => GetEuler(); set => SetEuler(value); }
		public Quaternion rotation { get => GetRotation(); set => SetRotation(value); }
		public Vector3 scale { get => GetScale(); set => SetScale(value); }
	%}
};