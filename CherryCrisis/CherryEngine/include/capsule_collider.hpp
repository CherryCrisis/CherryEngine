#pragma once

#include <cherry_macros.hpp>

#include "collider.hpp"
#include "maths.hpp"
#include "renderer.hpp"

class Mesh;
class Transform;


class CCENGINE_API CapsuleCollider : public Collider, public ARenderer
{
private:
	using floatProperty = CCProperty::ConstRefProperty<CapsuleCollider, float>;

	float m_entityScale			= 1.f;
	float m_entityRadius		= 1.f;
	float m_editableScale		= 1.f;
	float m_editableRadius		= 1.f;
	float m_totalRadius			= 1.f;
	float m_totalScale			= 1.f;

	CCMaths::Matrix4 m_topModel	= CCMaths::Matrix4::Identity;
	CCMaths::Matrix4 m_botModel	= CCMaths::Matrix4::Identity;

	void PopulateMetadatas() override;

public:
	CapsuleCollider();
	CapsuleCollider(CCUUID& id);
	~CapsuleCollider();

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
	
	void SetScale(const float& scale);
	float GetScale() { return m_editableScale; }

	void SetRadius(const float& radius);
	float GetRadius() { return m_editableRadius; }

	void RecomputeMatrix(const CCMaths::Vector3& ignored) override { ComputeModelMatrices(); }
	void ComputeModelMatrices() override;
	CCMaths::Matrix4 GetModelMatrix() override;
	CCMaths::Matrix4 GetTopMatrix();
	CCMaths::Matrix4 GetBotMatrix();

	void OnCellAdded(Cell* newCell);
	void OnCellRemoved(Cell* newCell);

	floatProperty editableScale{ this, &CapsuleCollider::SetScale, &CapsuleCollider::GetScale };
	floatProperty radius{ this, &CapsuleCollider::SetRadius, &CapsuleCollider::GetRadius };
};
