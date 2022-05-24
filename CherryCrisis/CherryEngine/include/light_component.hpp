#pragma once

#include <cherry_macros.hpp>

#include "behaviour.hpp"
#include "light.hpp"

#include "property.hpp"

class Cell;
class Transform;


class CCENGINE_API LightComponent : public Behaviour, public ARenderer
{
protected:
	void PopulateMetadatas() override;

	using Vector3Property = CCProperty::ConstRefProperty<LightComponent, CCMaths::Vector3>;
	using FloatProperty = CCProperty::CopyProperty<LightComponent, float>;

public:
	Light		m_light;

	float m_cutoff = CCMaths::PI;
	float m_outerCutoff = CCMaths::PI;

	Transform*	m_transform = nullptr;

	LightComponent();
	LightComponent(CCUUID& id);
	~LightComponent();

	void Initialize();
	void BindToSignals() override;

	void ChangePosition(const CCMaths::Vector3& position);
	void ChangeRotation(const CCMaths::Vector3& rotation);

	void SubscribeToPipeline(ARenderingPipeline* pipeline) override;
	void UnsubscribeToPipeline(ARenderingPipeline* pipeline) override;
	void OnCellAdded(Cell* newCell);
	void OnCellRemoved(Cell* newCell);

	void SubscribeGuizmo();
	void UnsubscribeGuizmo();

	void InvalidateTransform();

	void SetPosition(const CCMaths::Vector3& newPosition);
	void SetRotation(const CCMaths::Vector3& newDirection);
	void SetAmbient(const CCMaths::Vector3& newAmbient);
	void SetDiffuse(const CCMaths::Vector3& newDiffuse);
	void SetSpecular(const CCMaths::Vector3& newSpecular);
	void SetAttenuation(const CCMaths::Vector3& newAttenuation);
	void SetCutoff(float newCutoff);
	void SetOuterCutoff(float newOuterCutoff);
	void SetLightType(ELightType newType);

	CCMaths::Vector3 GetPosition() { return m_light.m_position; }
	CCMaths::Vector3 GetDirection() { return m_light.m_direction; }
	CCMaths::Vector3 GetAmbient() { return m_light.m_ambient; }
	CCMaths::Vector3 GetDiffuse() { return m_light.m_diffuse; }
	CCMaths::Vector3 GetSpecular() { return m_light.m_specular; }
	CCMaths::Vector3 GetAttenuation() { return m_light.m_attenuation; }
	float GetCutoff() { return m_cutoff; }
	float GetOuterCutoff() { return m_outerCutoff; }
	ELightType GetLightType() { return m_light.m_type; }

	Vector3Property position{ this, &LightComponent::SetPosition, &LightComponent::GetPosition };
	Vector3Property direction{ this, &LightComponent::SetRotation, &LightComponent::GetDirection };
	Vector3Property ambient{ this, &LightComponent::SetAmbient, &LightComponent::GetAmbient };
	Vector3Property diffuse{ this, &LightComponent::SetDiffuse, &LightComponent::GetDiffuse };
	Vector3Property specular{ this, &LightComponent::SetSpecular, &LightComponent::GetSpecular };
	Vector3Property attenuation{ this, &LightComponent::SetAttenuation, &LightComponent::GetAttenuation };
	FloatProperty cutoff{ this, &LightComponent::SetCutoff, &LightComponent::GetCutoff };
	FloatProperty outerCutoff{ this, &LightComponent::SetOuterCutoff, &LightComponent::GetOuterCutoff };

	CCProperty::CopyProperty<LightComponent, ELightType> lightType{ this, &LightComponent::SetLightType, &LightComponent::GetLightType };
};