%include "enums.swg"

%{
    #include "light_component.hpp"
    #include "maths.hpp"
%}

enum class ELightType : unsigned int
{
    OFF,
    POINT,
    DIRECTIONAL,
};

%include vector3.i
%include behaviour.i

%nodefaultctor LightComponent;

class LightComponent : public Behaviour
{
public:
	Transform*	m_transform = nullptr;

	void SetPosition(const CCMaths::Vector3& newPosition);
	void SetRotation(const CCMaths::Vector3& newDirection);
	void SetAmbient(const CCMaths::Vector3& newAmbient);
	void SetDiffuse(const CCMaths::Vector3& newDiffuse);
	void SetSpecular(const CCMaths::Vector3& newSpecular);
	void SetAttenuation(const CCMaths::Vector3& newAttenuation);
	void SetCutoff(float newCutoff);
	void SetOuterCutoff(float newOuterCutoff);
	void SetLightType(ELightType newType);

	CCMaths::Vector3 GetPosition();
	CCMaths::Vector3 GetDirection();
	CCMaths::Vector3 GetAmbient();
	CCMaths::Vector3 GetDiffuse();
	CCMaths::Vector3 GetSpecular();
	CCMaths::Vector3 GetAttenuation();
	float GetCutoff();
	float GetOuterCutoff();
	ELightType GetLightType();
};