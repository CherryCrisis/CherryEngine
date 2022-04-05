#pragma once

#include <vector>

#include <cherry_macros.hpp>

#include "behaviour.hpp"
#include "maths.hpp"

using namespace CCMaths;

class CCENGINE_API Transform : public Behaviour
{
private:
	Vector3 m_position = Vector3(0.f, 0.f, 0.f);
	Vector3 m_rotation = Vector3(0.f, 0.f, 0.f);
	Vector3 m_scale	   = Vector3(1.f, 1.f, 1.f);

	Transform* m_parent = nullptr;
	std::vector<Transform*> m_children;

	Matrix4 m_worldMatrix = Matrix4::Identity;

	bool m_isDirty = true;

	void SetDirty();

	void PopulateMetadatas() override;

	template<typename T>
	using TransformProperty = CCProperty::Property<Transform, T>;
	using Vector3Property = TransformProperty<Vector3>;

public:
	Transform(Entity& owner);
	Transform(CCUUID& owner);
	Transform() { PopulateMetadatas(); }

	bool IsRoot() { return !m_parent; }

	void SetParent(Transform*& transform);
	Transform* GetParent() { return m_parent; }

	//TransformProperty<Transform*> parent{ this, &Transform::SetParent, &Transform::GetParent };

	void UpdateMatrix();

	bool IsDirty() { return m_isDirty; }

	void SetPosition(Vector3& position);
	Vector3 GetPosition() { return m_position; }

	void SetRotation(Vector3& rotation);
	Vector3 GetRotation() { return m_rotation; }

	void SetScale(Vector3& scale);
	Vector3 GetScale() { return m_scale; }

	Vector3Property position{ this, &Transform::SetPosition, &Transform::GetPosition };
	Vector3Property rotation{ this, &Transform::SetRotation, &Transform::GetRotation };
	Vector3Property scale{ this, &Transform::SetScale, &Transform::GetScale };

	TransformProperty<Transform*> parent{ this, &Transform::SetParent, &Transform::GetParent };

	Event<const Vector3&> m_onPositionChange;
	Event<const Vector3&> m_onRotationChange;

	std::vector<Transform*> GetChildren() { return m_children; }
	void AddChildren(Transform* transform);

	Matrix4 GetWorldMatrix();
};