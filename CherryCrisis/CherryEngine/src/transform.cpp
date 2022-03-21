#include "pch.hpp"

#include "transform.hpp"

Transform::Transform(Entity& owner)
	: Behaviour(owner)
{

}


void Transform::SetDirty()
{
	m_isDirty = true;

	for (Transform* child : m_children)
		child->SetDirty();
}

void Transform::SetParent(Transform* transform)
{
	if (transform)
	{
		transform->m_children.push_back(this);
	}
	else
	{
		//To remove this transform in parent
		if (m_parent)
		{
			size_t childrenSize = m_parent->m_children.size();
			for (size_t i = 0; i < m_parent->m_children.size(); ++i)
			{
				if (this == m_parent->m_children[i])
				{
					m_parent->m_children[i] = m_parent->m_children[childrenSize - 1];
					m_parent->m_children.pop_back();
				}
			}
		}
	}
	m_parent = transform;
}

void Transform::UpdateMatrix()
{
	if (!m_isDirty)
		return;

	Matrix4 worldMatrix =
	Matrix4::Translate(m_position) *
	Matrix4::RotateZXY(m_rotation) *
	Matrix4::Scale(m_scale);

	if (m_parent)
		worldMatrix = m_parent->GetWorldMatrix() * worldMatrix;

	m_worldMatrix = worldMatrix;

	m_isDirty = false;
}

Matrix4 Transform::GetWorldMatrix()
{
	// TO DO: change this using event/cpp properties
	if (!m_isDirty)
		return m_worldMatrix;

	m_isDirty = false;

	m_worldMatrix = Matrix4::Translate(m_position) * Matrix4::RotateZXY(m_rotation) * Matrix4::Scale(m_scale);

	if (!m_parent)
		return m_worldMatrix;

	return m_worldMatrix = m_parent->GetWorldMatrix() * m_worldMatrix;
}

void Transform::SetPosition(const Vector3& position)
{
	m_position = position;
	SetDirty();
}

void Transform::SetRotation(const Vector3& rotation)
{
	m_rotation = rotation;
	SetDirty();
}

void Transform::SetScale(const Vector3& scale)
{
	m_scale = scale;
	SetDirty();
}

void Transform::AddChildren(Transform* transform)
{
	m_children.push_back(transform);
}

std::string Transform::Serialize()
{
	std::string value;
	value += std::to_string(m_position.x) + "/" + std::to_string(m_position.y) + "/"+  std::to_string(m_position.z) + "\n";
	value += "		"	+ std::to_string(m_rotation.x) + "/" + std::to_string(m_rotation.y) + "/" + std::to_string(m_rotation.z) + "\n";
	value += "		"   + std::to_string(m_scale.x) + "/" + std::to_string(m_scale.y) + "/" + std::to_string(m_scale.z) + "\n";
	return value;
}