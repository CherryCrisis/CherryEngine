#pragma once

#include <vector>
#include "maths.hpp"

using namespace CCMaths;

class Transform
{
private:
	Vector3 m_position;
	Vector3 m_rotation;
	Vector3 m_scale;

	Transform* m_parent;
	std::vector<Transform*> m_children;

	Matrix4 m_worldMatrix;
	bool m_hasChanged;

public:
	Transform();
	void SetParent(Transform* transform);
	void UpdateMatrix();

	bool HasChanged() { return m_hasChanged; }

	void SetPosition(const Vector3& position);
	Vector3 GetPosition() { return m_position; }

	void SetRotation(const Vector3& rotation);
	Vector3 GetRotation() { return m_rotation; }

	void SetScale(const Vector3& scale);
	Vector3 GetScale() { return m_scale; }

	void GetChildren(std::vector<Transform*>& transformChildren);
	void AddChildren(Transform* transform);

	Matrix4 GetWorldMatrix() { return m_worldMatrix; }
};