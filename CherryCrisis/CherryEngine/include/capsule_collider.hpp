#pragma once

#include "cherry_macros.hpp"

#include "maths.hpp"
#include "collider.hpp"

class Transform;

class CCENGINE_API CapsuleCollider : public Collider
{
private:
	using floatProperty = CCProperty::ConstRefProperty<CapsuleCollider, float>;

	physx::PxShape* m_pxShape = nullptr;

	float m_baseEntityScale =1.f;
	float m_entityScale = 1.f;
	float m_entityRadius = 1.f;
	float m_editableScale = 1.f;
	float m_editableRadius = 1.f;

	void PopulateMetadatas() override;

public:
	CapsuleCollider();
	~CapsuleCollider();

	void BindToSignals() override;
	void Unregister() override;

	void SetEntityScale(const Vector3& scale) override;
	void SetPxShape() override;
	void ClearPxShape() override;
	void ResetPxShape() override;

	void SetPxData() override;

	void SetScale(const float& scale) { m_editableScale = scale; SetPxData(); }
	float GetScale() { return m_editableScale; }

	void SetRadius(const float& radisu) { m_editableRadius = radisu; SetPxData(); }
	float GetRadius() { return m_editableRadius; }

	floatProperty editableScale{ this, &CapsuleCollider::SetScale, &CapsuleCollider::GetScale };
	floatProperty radius{ this, &CapsuleCollider::SetRadius, &CapsuleCollider::GetRadius };
};
