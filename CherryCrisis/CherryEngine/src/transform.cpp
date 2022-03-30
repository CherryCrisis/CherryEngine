#include "pch.hpp"

#include "transform.hpp"

Transform::Transform(Entity& owner)
	: Behaviour(owner)
{
	PopulateMetadatas();
}

Transform::Transform(CCUUID& owner)
	: Behaviour(owner)
{
	PopulateMetadatas();
}


void Transform::PopulateMetadatas() 
{
	m_metadatas.SetField<Behaviour*>("parent", m_parent);

	m_metadatas.SetProperty("position", &position);
	m_metadatas.SetProperty("rotation", &rotation);
	m_metadatas.SetProperty("scale", &scale);
	//m_metadatas.m_fields["parent"] = { "parent",  (Behaviour*)m_parent};
}

void Transform::ConsumeMetadatas()
{
	auto a = m_metadatas.m_fields["parent"].m_value;
	Behaviour* b =  std::any_cast<Behaviour*>(m_metadatas.m_fields["parent"].m_value);
	
	m_parent   = (Transform*)std::any_cast<Behaviour*>(m_metadatas.m_fields["parent"].m_value);
	/*m_position = std::any_cast<Vector3>(m_metadatas.m_fields["position"].m_value);
	m_rotation = std::any_cast<Vector3>(m_metadatas.m_fields["rotation"].m_value);
	m_scale    = std::any_cast<Vector3>(m_metadatas.m_fields["scale"].m_value);*/
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
	m_metadatas.SetField<Behaviour*>("parent", m_parent);
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

void Transform::SetPosition(Vector3& position)
{
	m_position = position;
	SetDirty();
}

void Transform::SetRotation(Vector3& rotation)
{
	m_rotation = rotation;
	SetDirty();
}

void Transform::SetScale(Vector3& scale)
{
	m_scale = scale;
	SetDirty();
}

void Transform::AddChildren(Transform* transform)
{
	m_children.push_back(transform);
}
/*
std::string Transform::Serialize()
{
	std::string value;
	value += std::to_string(m_position.x) + "/" + std::to_string(m_position.y) + "/"+  std::to_string(m_position.z) + "\n";
	value += "  "	+ std::to_string(m_rotation.x) + "/" + std::to_string(m_rotation.y) + "/" + std::to_string(m_rotation.z) + "\n";
	value += "  "   + std::to_string(m_scale.x) + "/" + std::to_string(m_scale.y) + "/" + std::to_string(m_scale.z) + "\n";
	if (m_parent)
		value += "  m_parent: " + std::to_string(m_parent->GetUUID());
	else
		value += "  m_parent: null";
	return value;
}*/
