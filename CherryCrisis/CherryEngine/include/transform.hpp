#pragma once

#include <vector>
#include "maths.hpp"

#include "CherryMacros.h"

using namespace CCMaths;

class CCENGINE_API Transform
{
private:
	Vector3 m_position = Vector3(0.f, 0.f, 0.f);
	Vector3 m_rotation = Vector3(0.f, 0.f, 0.f);
	Vector3 m_scale	   = Vector3(1.f, 1.f, 1.f);

	Transform* m_parent = nullptr;
	std::vector<Transform*> m_children;

	Matrix4 m_worldMatrix = Matrix4::Identity();

	bool m_isDirty = true;

	void SetDirty();

public:
	void SetParent(Transform* transform);
	void UpdateMatrix();

	bool IsDirty() { return m_isDirty; }

	void SetPosition(const Vector3& position);
	Vector3 GetPosition() { return m_position; }

	void SetRotation(const Vector3& rotation);
	Vector3 GetRotation() { return m_rotation; }

	void SetScale(const Vector3& scale);
	Vector3 GetScale() { return m_scale; }

	void GetChildren(std::vector<Transform*>& transformChildren);
	void AddChildren(Transform* transform);

	Matrix4 GetWorldMatrix();
};