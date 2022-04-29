#include "pch.hpp"

#include "transform.hpp"

#include <algorithm>

Transform::Transform()
{
	PopulateMetadatas();
}

Transform::Transform(CCUUID& id)
	: Behaviour(id)
{
	PopulateMetadatas();
}

Transform::~Transform()
{
	SetParent(nullptr);
	
	while (!m_children.empty())
		ClearChildParenting();
}


void Transform::PopulateMetadatas() 
{
	Behaviour::PopulateMetadatas();

	m_metadatas.SetProperty("position", &position);
	m_metadatas.SetProperty("rotation", &rotation);
	m_metadatas.SetProperty("scale", &scale);
	m_metadatas.SetProperty("parent", &parent);
}

void Transform::SetDirty()
{
	m_isDirty = true;

	for (Transform* child : m_children)
		child->SetDirty();
}

bool Transform::IsEqualToParent(Transform* transform)
{
	if (!m_parent)
		return false;

	if (m_parent == transform)
		return true;

	return m_parent->IsEqualToParent(transform);
}

void Transform::SetParent(Transform* transform)
{
	if (IsEqualToParent(transform)) // Guard to prevent parent re-set
		return;

	if (transform && transform->IsEqualToParent(this))
		return;

	if (m_parent) // Remove the transform from the last parent
	{
		size_t childrenSize = m_parent->m_children.size();
		for (size_t i = 0; i < childrenSize; ++i)
		{
			if (this == m_parent->m_children[i])
			{
				m_parent->m_children[i] = m_parent->m_children.back();
				m_parent->m_children.pop_back();
				break;
			}
		}
	}
	if (transform) // Subscribe to the new parent if there is one
	{
		transform->m_children.push_back(this);
	}

	m_parent = transform;

	ReapplyPosition();
	ReapplyRotation();
	ReapplyScale();

	SetDirty();
}

void Transform::ClearChildParenting()
{
	size_t childrenSize = m_children.size();
	for (size_t i = 0; i < childrenSize; ++i)
	{
		m_children[i]->SetParent(nullptr);
		break;
	}
}

Transform* Transform::GetRootParent() 
{
	Transform* rootParent = this;

	if (m_parent)
		rootParent = m_parent->GetRootParent();

	return rootParent;
}

void Transform::UpdateMatrix()
{
	if (!m_isDirty)
		return;

	Matrix4 worldMatrix =
	Matrix4::Translate(m_position) *
	Matrix4::RotateXYZ(m_rotation) *
	Matrix4::Scale(m_scale);

	if (m_parent)
		worldMatrix = m_parent->GetWorldMatrix() * worldMatrix;

	m_worldMatrix = worldMatrix;

	m_isDirty = false;
}

Matrix4 Transform::GetWorldMatrix()
{
	// TODO: change this using event/cpp properties
	if (!m_isDirty)
		return m_worldMatrix;

	m_isDirty = false;

	m_worldMatrix = Matrix4::Translate(m_position) * Matrix4::RotateZYX(m_rotation) * Matrix4::Scale(m_scale);

	if (!m_parent)
		return m_worldMatrix;

	return m_worldMatrix = m_parent->GetWorldMatrix() * m_worldMatrix;
}

Matrix4 Transform::GetLocalMatrix()
{
	if (!m_parent)
		return m_worldMatrix;

	return CCMaths::Matrix4::Inverse(m_parent->GetWorldMatrix()) * m_worldMatrix;
}

void Transform::SetPosition(const Vector3& position)
{
	m_position = position;
	SetDirty();
	m_onPositionChange.Invoke(position);
}

void Transform::SetRotation(const Vector3& rotation)
{
	m_rotation = rotation;
	SetDirty();
	m_onRotationChange.Invoke(rotation);
}

void Transform::SetScale(const Vector3& scale)
{
	m_scale = scale;
	SetDirty();
	m_onScaleChange.Invoke(scale);
}

void Transform::ReapplyPosition()
{
	CCMaths::Vector3 tempRot;
	CCMaths::Vector3 tempScale;

	CCMaths::Matrix4::Decompose(GetLocalMatrix(), m_position, tempRot, tempScale);
	
	SetDirty();
	m_onPositionChange.Invoke(m_position);
}

Vector3 Transform::GetGlobalPosition()
{
	CCMaths::Vector3 tempRot;
	CCMaths::Vector3 tempScale;

	CCMaths::Vector3 outPos;

	CCMaths::Matrix4::Decompose(GetWorldMatrix(), outPos, tempRot, tempScale);

	return outPos;
}

void Transform::ReapplyRotation()
{
	CCMaths::Vector3 tempPos;
	CCMaths::Vector3 tempScale;

	CCMaths::Matrix4::Decompose(GetLocalMatrix(), tempPos, m_rotation, tempScale);

	SetDirty();
	m_onRotationChange.Invoke(m_rotation);
}

Vector3 Transform::GetGlobalRotation()
{
	CCMaths::Vector3 tempPos;
	CCMaths::Vector3 tempScale;

	CCMaths::Vector3 outRot;

	CCMaths::Matrix4::Decompose(GetWorldMatrix(), tempPos, outRot, tempScale);

	return outRot;
}

void Transform::ReapplyScale()
{
	CCMaths::Vector3 tempPos;
	CCMaths::Vector3 tempRot;

	CCMaths::Matrix4::Decompose(GetLocalMatrix(), tempPos, tempRot, m_scale);

	SetDirty();
}

Vector3 Transform::GetGlobalScale()
{
	CCMaths::Vector3 tempPos;
	CCMaths::Vector3 tempRot;

	CCMaths::Vector3 outScale;

	CCMaths::Matrix4::Decompose(GetWorldMatrix(), tempPos, tempRot, outScale);

	return outScale;
}

void Transform::AddChildren(Transform* transform)
{
	m_children.push_back(transform);
}

bool Transform::IsChildOf(Transform* parent, bool recursive) 
{
	bool value = false;

	std::vector<Transform*> childrens = parent->GetChildren();
	if (childrens.size() > 0) 
	{
		if (std::find(childrens.begin(), childrens.end(), this) != childrens.end()) 
		{
			return true;
		}
		if (recursive) 
		{
			for (Transform* t : childrens)
			{
				value |= IsChildOf(t, true);
			}
		}
		return value;
	}
	return false;
}

void Transform::BindToSignals()
{
	GetHost().m_OnCellAdded.Bind(&Transform::OnCellAdded, this);
	GetHost().m_OnCellRemoved.Bind(&Transform::OnCellRemoved, this);
}

void Transform::OnCellAdded(Cell* newCell)
{
	for (Transform* child : m_children)
		child->GetHost().m_OnCellAdded.Invoke(std::forward<Cell*>(newCell));
}

void Transform::OnCellRemoved(Cell* newCell)
{
	for (Transform* child : m_children)
		child->GetHost().m_OnCellRemoved.Invoke(std::forward<Cell*>(newCell));
}
