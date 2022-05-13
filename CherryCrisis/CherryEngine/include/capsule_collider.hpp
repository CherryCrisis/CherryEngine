#pragma once

#include "cherry_macros.hpp"

#include "maths.hpp"
#include "collider.hpp"
#include "renderer.hpp"

class Transform;
class Mesh;

class CCENGINE_API CapsuleCollider : public Collider, public ARenderer
{
private:
	using floatProperty = CCProperty::ConstRefProperty<CapsuleCollider, float>;

	float m_baseEntityScale =1.f;
	float m_entityScale = 1.f;
	float m_entityRadius = 1.f;
	float m_editableScale = 1.f;
	float m_editableRadius = 1.f;

	std::shared_ptr<Mesh> m_capsTopCollider = nullptr;
	std::shared_ptr<Mesh> m_capsBodyCollider = nullptr;

	void PopulateMetadatas() override;

public:
	CapsuleCollider();
	CapsuleCollider(CCUUID& id);
	~CapsuleCollider();

	void BindToSignals() override;
	void Unregister() override;

	void SetEntityScale(const CCMaths::Vector3& scale) override;
	void SetPxShape() override;
	void ClearPxShape() override;
	void ResetPxShape() override;

	void SetPxData() override;

	void SubscribeToPipeline(ARenderingPipeline* pipeline) override;
	void UnsubscribeToPipeline(ARenderingPipeline* pipeline) override;

	void SetScale(const float& scale) { m_editableScale = scale; ResetPxShape(); }
	float GetScale() { return m_editableScale; }

	void SetRadius(const float& radius) { m_editableRadius = radius; ResetPxShape(); }
	float GetRadius() { return m_editableRadius; }

	floatProperty editableScale{ this, &CapsuleCollider::SetScale, &CapsuleCollider::GetScale };
	floatProperty radius{ this, &CapsuleCollider::SetRadius, &CapsuleCollider::GetRadius };
};
