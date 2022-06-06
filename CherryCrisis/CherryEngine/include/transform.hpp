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

	Vector3	m_position			= Vector3::Zero;
	Vector3	m_scale				= Vector3::One;
	Quaternion m_rotation		= Quaternion::Identity;

	Vector3 m_up				= Vector3::Zero;
	Vector3 m_right				= Vector3::Zero;
	Vector3 m_forward			= Vector3::Zero;

	Vector3	m_worldPosition		= Vector3::Zero;
	Vector3	m_worldScale		= Vector3::One;
	Quaternion m_worldRotation	= Quaternion::Identity;
	
	Matrix4 m_worldMatrix = Matrix4::Identity;

	std::vector<Transform*> m_children;
	
	Transform* m_parent = nullptr;

	void SetDirty(int dirtyFlag);

	void PopulateMetadatas() override;

public:
	Event<Transform*>	m_onPositionChange;
	Event<Transform*>	m_onRotationChange;
	Event<Transform*>	m_onScaleChange;
	Event<Transform*>	m_onTransformEdited;
	Event<>				m_onDestroyed;

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
	void OnParentDestroyed();

	bool IsDirty() { return m_isDirty; }

	void SetPosition(const Vector3& position);
	void SetGlobalPosition(const Vector3& position);
	Vector3 GetPosition() { return m_position; }
	Vector3 GetGlobalPosition();

	void SetEuler(const Vector3& rotation);
	void SetRotation(const Quaternion& rotation);
	void SetGlobalRotation(const Vector3& rotation);
	void SetGlobalRotation(const Quaternion& rotation);
	Vector3 GetEuler() { return Quaternion::ToEuler(m_rotation); }
	Quaternion GetRotation() { return m_rotation; }
	Quaternion GetGlobalRotation();

	void SetScale(const Vector3& scale);
	void SetGlobalScale(const Vector3& scale);
	Vector3 GetScale() { return m_scale; }
	Vector3 GetGlobalScale();

	void Translate(const Vector3& translation);
	void Rotate(const Vector3& eulerAngles);
	void Rotate(const Vector3& axis, float angle);

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

	Vector3 Forward()	{ GetWorldMatrix();  return m_forward; }
	Vector3 Right()		{ GetWorldMatrix();  return m_right; }
	Vector3 Up()		{ GetWorldMatrix();  return m_up; }

	Vector3Property position{ this, &Transform::SetPosition, &Transform::GetPosition };
	Vector3Property rotation{ this, &Transform::SetEuler, &Transform::GetEuler };
	Vector3Property scale{ this, &Transform::SetScale, &Transform::GetScale };

	CCProperty::CopyProperty<Transform, Transform*> parent{ this, &Transform::SetParent, &Transform::GetParent };
};
