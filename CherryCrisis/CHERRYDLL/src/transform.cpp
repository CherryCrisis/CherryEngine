#include "transform.hpp"

Transform::Transform()
	: m_worldMatrix(Matrix4::Identity()), m_hasChanged(true), m_parent(nullptr)
{
	m_position = { 0.f };
	m_rotation = { 0.f };
	m_scale = { 0.f };
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
	if (!HasChanged())
		return;

	Matrix4 worldMatrix =
	Matrix4::Translate(m_position) *
	Matrix4::Rotate(m_rotation) *
	Matrix4::Scale(m_scale);

	if (m_parent)
		worldMatrix = m_parent->m_worldMatrix * worldMatrix;

	m_worldMatrix = worldMatrix;
}

void Transform::SetPosition(const Vector3& position)
{
	m_position = position;
	m_hasChanged = true;
}

void Transform::SetRotation(const Vector3& rotation)
{
	m_rotation = rotation;
	m_hasChanged = true;
}

void Transform::SetScale(const Vector3& scale)
{
	m_scale = scale;
	m_hasChanged = true;
}

void Transform::GetChildren(std::vector<Transform*>& transformChildren)
{
	transformChildren.resize(m_children.size());
	std::copy(m_children.begin(), m_children.end(), transformChildren.data());
}

void Transform::AddChildren(Transform* transform)
{
	m_children.push_back(transform);
}