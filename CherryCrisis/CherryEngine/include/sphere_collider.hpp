#pragma once

#include <cherry_macros.hpp>

#include "maths.hpp"
#include "collider.hpp"
#include "renderer.hpp"

class Transform;
class Mesh;

class CCENGINE_API SphereCollider : public Collider, public ARenderer
{
private:
	using floatProperty = CCProperty::ConstRefProperty<SphereCollider, float>;

	float m_editableScale	= 1.f;
	float m_entityScale		= 1.f;
	float m_totalScale		= 1.f;

	CCMaths::Matrix4 m_model = CCMaths::Matrix4::Identity;

	void PopulateMetadatas() override;

public:
	SphereCollider();
	SphereCollider(CCUUID& id);
	~SphereCollider();

	void BindToSignals() override;
	void Initialize() override;
	void InvalidateTransform() override;
	void Unregister(bool checkEmpty = true) override;

	void SetAABBScale();
	void SetEntityScale(Transform* transform) override;
	void SetPxShape() override;
	void ClearPxShape() override;
	void ResetPxShape() override;

	void SetPxData() override;

	void SubscribeToPipeline(ARenderingPipeline* pipeline) override;
	void UnsubscribeToPipeline(ARenderingPipeline* pipeline) override;

	void Copy(Behaviour* copy) override;

	void Visible() override { this->Collider::Visible(); }
	void Unvisible() override { this->Collider::Unvisible(); }

	void SetScale(const float& scale);
	float GetScale() { return m_editableScale; }

	void RecomputeMatrix(Transform* transform) override { ComputeModelMatrices(); }
	void ComputeModelMatrices() override;
	CCMaths::Matrix4 GetModelMatrix() override;

	void OnCellAdded(Cell* newCell);
	void OnCellRemoved(Cell* newCell);

	floatProperty editableScale{ this, &SphereCollider::SetScale, &SphereCollider::GetScale };
};
