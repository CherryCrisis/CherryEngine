#pragma once

#include "cherry_macros.hpp"

#include "maths.hpp"
#include "collider.hpp"

class Transform;

class CCENGINE_API SphereCollider : public Collider
{
private:
	physx::PxShape* m_pxShape = nullptr;

	float m_baseEntityScale = 1.f;
	float m_editableScale = 1.f;

	using floatProperty = CCProperty::ConstRefProperty<SphereCollider, float>;

	void PopulateMetadatas() override;

public:
	SphereCollider();
	~SphereCollider();

	void BindToSignals() override;
	void Unregister() override;

	void SetPxShape() override;
	void ClearPxShape() override;
	void ResetPxShape() override;

	void SetPxData() override;

	void SetScale(const float& scale) { m_editableScale = scale; SetPxData(); }
	float GetScale() { return m_editableScale; }

	floatProperty editableScale{ this, &SphereCollider::SetScale, &SphereCollider::GetScale };
};
