#pragma once

#include <vector>

#include <cherry_macros.hpp>

#include "behaviour.hpp"
#include "maths.hpp"

using namespace CCMaths;

class Cell;


class CCENGINE_API Transform : public Behaviour
{
	enum class EDirtyFlag
	{
		WORLD_MATRIX = 1 << 0,
		WORLD_POSITION = 1 << 1,
		WORLD_ROTATION = 1 << 2,
		WORLD_SCALE = 1 << 3,
	};

private:
	using Vector3Property = CCProperty::ConstRefProperty<Transform, Vector3>;

	unsigned char m_isDirty = (int)EDirtyFlag::WORLD_MATRIX |
							  (int)EDirtyFlag::WORLD_POSITION |
						      (int)EDirtyFlag::WORLD_ROTATION |
							  (int)EDirtyFlag::WORLD_SCALE;

	Vector3 m_position = CCMaths::Vector3::Zero;
	Vector3 m_rotation = CCMaths::Vector3::Zero;
	Vector3 m_scale	   = CCMaths::Vector3::One;

	Vector3 m_up		= CCMaths::Vector3::Zero;
	Vector3 m_right		= CCMaths::Vector3::Zero;
	Vector3 m_forward	= CCMaths::Vector3::Zero;

	Vector3 m_worldPosition = Vector3(0.f, 0.f, 0.f);
	Vector3 m_worldRotation = Vector3(0.f, 0.f, 0.f);
	Vector3 m_worldScale = Vector3(1.f, 1.f, 1.f);
	
	Matrix4 m_worldMatrix = Matrix4::Identity;

	std::vector<Transform*> m_children;
	
	Transform* m_parent = nullptr;

	void SetDirty(int dirtyFlag);

	void PopulateMetadatas() override;

public:
	Event<const Vector3&>	m_onPositionChange;
	Event<const Vector3&>	m_onRotationChange;
	Event<const Vector3&>	m_onScaleChange;
	Event<Transform*>		m_onTransformEdited;

	Transform();
	Transform(CCUUID& owner);
	~Transform();
	bool IsRoot() { return !m_parent; }

	bool CanDelete() override;
	void SetParent(Transform* parent, bool reapplyPosition, bool reapplyRot = false, bool reapplyScale = false);
	void SetParent(Transform* parent);
	Transform* GetParent() { return m_parent; }
	Transform* GetRootParent();
	bool IsEqualToParent(Transform* transform);

	bool IsDirty() { return m_isDirty; }

	void SetPosition(const Vector3& position);
	Vector3 GetPosition() { return m_position; }
	Vector3 GetGlobalPosition();

	void SetRotation(const Vector3& rotation);
	Vector3 GetRotation() { return m_rotation; }
	Vector3 GetGlobalRotation();

	void SetScale(const Vector3& scale);
	Vector3 GetScale() { return m_scale; }
	Vector3 GetGlobalScale();

	void AddChildren(Transform* transform);
	std::vector<Transform*>* GetChildren() { return &m_children; }

	void BindToSignals() override;

	void OnCellAdded(Cell* newCell);
	void OnCellRemoved(Cell* newCell);

	bool IsChildOf(Transform* parent, bool recursive=true);
	void Copy(Behaviour* copy) override;

	Matrix4 GetWorldMatrix();
	void SetWorldMatrix(const CCMaths::Matrix4& worldMatrix);

	Matrix4 GetLocalMatrix();

	Vector3 Forward()	{ return m_forward; }
	Vector3 Right()		{ return m_right; }
	Vector3 Up()		{ return m_up; }

	Vector3Property position{ this, &Transform::SetPosition, &Transform::GetPosition };
	Vector3Property rotation{ this, &Transform::SetRotation, &Transform::GetRotation };
	Vector3Property scale{ this, &Transform::SetScale, &Transform::GetScale };

	CCProperty::CopyProperty<Transform, Transform*> parent{ this, &Transform::SetParent, &Transform::GetParent };
};
