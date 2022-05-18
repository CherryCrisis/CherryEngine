#include "pch.hpp"

#include "transform.hpp"

#include <algorithm>

#include "entity.hpp"
#include "event.hpp"
#include "cell.hpp"

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

void Transform::SetDirty(int dirtyFlag)
{
	m_isDirty = true;

	for (Transform* child : m_children)
		child->SetDirty((int)EDirtyFlag::WORLD_MATRIX | (int)EDirtyFlag::WORLD_POSITION | (int)EDirtyFlag::WORLD_ROTATION | (int)EDirtyFlag::WORLD_SCALE);
}

bool Transform::IsEqualToParent(Transform* transform)
{
	if (!m_parent)
		return false;

	if (m_parent == transform)
		return true;

	return m_parent->IsEqualToParent(transform);
}

void Transform::SetParent(Transform* parent, bool reapplyPosition, bool reapplyRotation, bool reapplyScale)
{
	if (IsEqualToParent(parent)) // Guard to prevent parent re-set
		return;

	if (parent && parent->IsEqualToParent(this))
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
	if (parent) // Subscribe to the new parent if there is one
	{
		parent->m_children.push_back(this);
	}

	m_parent = parent;

	if (reapplyPosition) ReapplyPosition();
	if (reapplyRotation) ReapplyRotation();
	if (reapplyScale)    ReapplyScale();

	SetDirty((int)EDirtyFlag::WORLD_MATRIX | (int)EDirtyFlag::WORLD_POSITION | (int)EDirtyFlag::WORLD_ROTATION | (int)EDirtyFlag::WORLD_SCALE);
}

void Transform::SetParent(Transform* transform)
{
	SetParent(transform, false, false, false);
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

Matrix4 Transform::GetWorldMatrix()
{
	// TODO: change this using event/cpp properties
	if (!(m_isDirty & (int)EDirtyFlag::WORLD_MATRIX))
		return m_worldMatrix;

	m_isDirty &= ~(int)EDirtyFlag::WORLD_MATRIX;

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
	m_onTransformEdited.Invoke(this);
	m_position = position;
	SetDirty((int)EDirtyFlag::WORLD_MATRIX | (int)EDirtyFlag::WORLD_POSITION);
	m_onPositionChange.Invoke(position);

	for (Transform* child : m_children)
		child->m_onPositionChange.Invoke(child->GetPosition());
}

void Transform::SetRotation(const Vector3& rotation)
{
	m_onTransformEdited.Invoke(this);
	m_rotation = rotation;
	SetDirty((int)EDirtyFlag::WORLD_MATRIX | (int)EDirtyFlag::WORLD_ROTATION);
	m_onRotationChange.Invoke(rotation);

	for (Transform* child : m_children)
		child->m_onRotationChange.Invoke(child->GetRotation());
}

void Transform::SetScale(const Vector3& scale)
{
	m_onTransformEdited.Invoke(this);
	m_scale = scale;
	SetDirty((int)EDirtyFlag::WORLD_MATRIX | (int)EDirtyFlag::WORLD_SCALE);
	m_onScaleChange.Invoke(scale);

	for (Transform* child : m_children)
		child->m_onScaleChange.Invoke(child->GetScale());
}

void Transform::ReapplyPosition()
{
	CCMaths::Vector3 tempRot;
	CCMaths::Vector3 tempScale;

	CCMaths::Matrix4::Decompose(GetLocalMatrix(), m_position, tempRot, tempScale);
	
	SetDirty((int)EDirtyFlag::WORLD_MATRIX | (int)EDirtyFlag::WORLD_POSITION);
	m_onPositionChange.Invoke(m_position);
}

void Transform::ReapplyRotation()
{
	CCMaths::Vector3 tempPos;
	CCMaths::Vector3 tempScale;

	CCMaths::Matrix4::Decompose(GetLocalMatrix(), tempPos, m_rotation, tempScale);

	SetDirty((int)EDirtyFlag::WORLD_MATRIX | (int)EDirtyFlag::WORLD_ROTATION);
	m_onRotationChange.Invoke(m_rotation);
}

void Transform::ReapplyScale()
{
	CCMaths::Vector3 tempPos;
	CCMaths::Vector3 tempRot;

	CCMaths::Matrix4::Decompose(GetLocalMatrix(), tempPos, tempRot, m_scale);

	SetDirty((int)EDirtyFlag::WORLD_MATRIX | (int)EDirtyFlag::WORLD_SCALE);
	m_onScaleChange.Invoke(m_scale);
}

void Transform::AddChildren(Transform* transform)
{
	m_children.push_back(transform);
}

bool Transform::IsChildOf(Transform* parent, bool recursive) 
{
	bool value = false;

	auto childrens = parent->GetChildren();
	if (childrens->size() > 0) 
	{
		if (std::find(childrens->begin(), childrens->end(), this) != childrens->end())
		{
			return true;
		}
		if (recursive) 
		{
			for (Transform* t : *childrens)
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

Vector3 Transform::GetGlobalPosition()
{
	if (!m_isDirty)
		return m_worldPosition;

	CCMaths::Matrix4::Decompose(GetWorldMatrix(), m_worldPosition, m_worldRotation, m_worldScale);

	m_isDirty &= ~((int)EDirtyFlag::WORLD_POSITION | (int)EDirtyFlag::WORLD_ROTATION | (int)EDirtyFlag::WORLD_SCALE);

	return m_worldPosition;
}

Vector3 Transform::GetGlobalRotation()
{
	if (!m_isDirty)
		return m_worldRotation;

	CCMaths::Matrix4::Decompose(GetWorldMatrix(), m_worldPosition, m_worldRotation, m_worldScale);

	m_isDirty &= ~((int)EDirtyFlag::WORLD_POSITION | (int)EDirtyFlag::WORLD_ROTATION | (int)EDirtyFlag::WORLD_SCALE);

	return m_worldRotation;
}

Vector3 Transform::GetGlobalScale()
{
	if (!m_isDirty)
		return m_worldScale;

	CCMaths::Matrix4::Decompose(GetWorldMatrix(), m_worldPosition, m_worldRotation, m_worldScale);

	m_isDirty &= ~((int)EDirtyFlag::WORLD_POSITION | (int)EDirtyFlag::WORLD_ROTATION | (int)EDirtyFlag::WORLD_SCALE);

	return m_worldScale;
}