#pragma once

#include "cherry_macros.hpp"

#include "maths.hpp"
#include "collider.hpp"
#include "renderer.hpp"

class Transform;
class Mesh;

class CCENGINE_API BoxCollider : public Collider, public ARenderer
{
private:
	using Vector3Property = CCProperty::ConstRefProperty<BoxCollider, CCMaths::Vector3>;

	CCMaths::Vector3 m_baseEntityScale = CCMaths::Vector3::One;
	CCMaths::Vector3 m_entityScale = CCMaths::Vector3::One;
	CCMaths::Vector3 m_editableScale = CCMaths::Vector3::One;
	CCMaths::Vector3 m_totalScale = CCMaths::Vector3::One;

	void PopulateMetadatas() override;

public:
	BoxCollider();
	BoxCollider(CCUUID& id);
	~BoxCollider();

	void BindToSignals() override;
	void Initialize() override;
	void InvalidateTransform() override;
	void Unregister() override;

	void SetEntityScale(const CCMaths::Vector3& scale) override;
	void SetPxShape() override;
	void ClearPxShape() override;
	void ResetPxShape() override;

	void SetPxData() override;

	void SubscribeToPipeline(ARenderingPipeline* pipeline) override;
	void UnsubscribeToPipeline(ARenderingPipeline* pipeline) override;

	void Visible() override { this->Collider::Visible(); }
	void Unvisible() override { this->Collider::Unvisible(); }

	void SetScale(const CCMaths::Vector3& scale);
	CCMaths::Vector3 GetScale() { return m_editableScale; }

	void RecomputeMatrix(const CCMaths::Vector3& ignored) override { ComputeModelMatrices(); }
	void ComputeModelMatrices() override;
	CCMaths::Matrix4 GetModelMatrix() override;

	Vector3Property editableScale{ this, &BoxCollider::SetScale, &BoxCollider::GetScale };
};
