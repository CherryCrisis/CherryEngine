#pragma once

#include "cherry_macros.hpp"

#include "maths.hpp"
#include "collider.hpp"

class Transform;

class CCENGINE_API BoxCollider : public Collider
{
private:
	using Vector3Property = CCProperty::ConstRefProperty<BoxCollider, CCMaths::Vector3>;

	physx::PxShape* m_pxShape = nullptr;

	CCMaths::Vector3 m_baseEntityScale = CCMaths::Vector3::One;
	CCMaths::Vector3 m_entityScale = CCMaths::Vector3::One;
	CCMaths::Vector3 m_editableScale = CCMaths::Vector3::One;

	void PopulateMetadatas() override;

public:
	BoxCollider();
	~BoxCollider();

	void BindToSignals() override;
	void Unregister() override;

	void SetEntityScale(const CCMaths::Vector3& scale) override;
	void SetPxShape() override;
	void ClearPxShape() override;
	void ResetPxShape() override;

	void SetPxData() override;

	void SetScale(const CCMaths::Vector3& scale) { m_editableScale = scale; ResetPxShape(); }
	CCMaths::Vector3 GetScale() { return m_editableScale; }

	Vector3Property editableScale{ this, &BoxCollider::SetScale, &BoxCollider::GetScale };
};
