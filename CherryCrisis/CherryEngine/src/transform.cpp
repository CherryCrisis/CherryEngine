#include <pch.hpp>

#include "transform.hpp"

#include <algorithm>

#include "scene_manager.hpp"

#include "cell.hpp"
#include "entity.hpp"
#include "event.hpp"


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
}


void Transform::PopulateMetadatas() 
{
	Behaviour::PopulateMetadatas();

	m_metadatas.SetProperty("position", &position);
	m_metadatas.SetProperty("rotation", &rotation, "toDegree");
	m_metadatas.SetProperty("scale", &scale);
	m_metadatas.SetProperty("parent", &parent);
}

void Transform::SetDirty(int dirtyFlag)
{
	m_isDirty = dirtyFlag;

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

bool Transform::CanDelete()
{
	if (m_children.empty())
		return true;

	return false;
}

void Transform::SetParent(Transform* parent, bool reapplyPosition, bool reapplyRotation, bool reapplyScale)
{
	if (IsEqualToParent(parent)) // Guard to prevent parent re-set
		return;

	if (parent && parent->IsEqualToParent(this))
		return;

	Matrix4 current = GetWorldMatrix();

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

	if (m_parent)
		current = CCMaths::Matrix4::Inverse(m_parent->GetWorldMatrix()) * m_worldMatrix;
	
	CCMaths::Vector3 tempPos;
	CCMaths::Vector3 tempRot;
	CCMaths::Vector3 tempScale;
	CCMaths::Matrix4::Decompose(GetLocalMatrix(), tempPos, tempRot, tempScale);
	
	if (reapplyPosition) SetPosition(tempPos);
	if (reapplyRotation) SetEuler(tempRot);
	if (reapplyScale)    SetScale(tempScale);

	SetDirty((int)EDirtyFlag::WORLD_MATRIX | (int)EDirtyFlag::WORLD_POSITION | (int)EDirtyFlag::WORLD_ROTATION | (int)EDirtyFlag::WORLD_SCALE);
	SceneManager::SetHierarchyDirty();
}

void Transform::SetParent(Transform* transform)
{
	SetParent(transform, false, false, false);
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

	m_worldMatrix = Matrix4::Translate(m_position) * Quaternion::ToMatrix(m_rotation) * Matrix4::Scale(m_scale);

	if (m_parent)
		m_worldMatrix = m_parent->GetWorldMatrix() * m_worldMatrix;

	m_up		= m_worldMatrix.up;
	m_right		= m_worldMatrix.right;
	m_forward	= -m_worldMatrix.back;

	Vector3 rotation;
	CCMaths::Matrix4::Decompose(m_worldMatrix, m_worldPosition, rotation, m_worldScale);
	m_worldRotation = Quaternion::FromEuler(rotation);

	return m_worldMatrix;
}

void Transform::SetWorldMatrix(const CCMaths::Matrix4& worldMatrix)
{
	m_worldMatrix = worldMatrix;
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
	m_onPositionChange.Invoke(this);

	for (Transform* child : m_children)
		child->m_onPositionChange.Invoke(&*child);
}

void Transform::SetGlobalPosition(const Vector3& position)
{
	Matrix4 world = Matrix4::Translate(position) * Quaternion::ToMatrix(m_worldRotation) * Matrix4::Scale(GetGlobalScale());

	if (m_parent)
		world = CCMaths::Matrix4::Inverse(m_parent->GetWorldMatrix()) * world;

	SetPosition(world.position);
}

void Transform::SetEuler(const Vector3& rotation)
{
	SetRotation(Quaternion::FromEuler(rotation));
}

void Transform::SetRotation(const Quaternion& rotation)
{
	m_onTransformEdited.Invoke(this);
	m_rotation = rotation;
	SetDirty((int)EDirtyFlag::WORLD_MATRIX | (int)EDirtyFlag::WORLD_ROTATION);
	m_onRotationChange.Invoke(this);

	for (Transform* child : m_children)
		child->m_onRotationChange.Invoke(&*child);
}

void Transform::SetGlobalRotation(const Vector3& rotation)
{
	SetGlobalRotation(Quaternion::FromEuler(rotation));
}

void Transform::SetGlobalRotation(const Quaternion& rotation)
{
	Matrix4 world = Matrix4::Translate(GetGlobalPosition()) * Quaternion::ToMatrix(rotation) * Matrix4::Scale(GetGlobalScale());

	if (m_parent)
		world = CCMaths::Matrix4::Inverse(m_parent->GetWorldMatrix()) * world;

	CCMaths::Vector3 tempPos;
	CCMaths::Vector3 tempRot;
	CCMaths::Vector3 tempScale;
	CCMaths::Matrix4::Decompose(world, tempPos, tempRot, tempScale);

	SetEuler(tempRot);
}

void Transform::SetScale(const Vector3& scale)
{
	m_onTransformEdited.Invoke(this);
	m_scale = scale;
	SetDirty((int)EDirtyFlag::WORLD_MATRIX | (int)EDirtyFlag::WORLD_SCALE);
	m_onScaleChange.Invoke(this);

	for (Transform* child : m_children)
		child->m_onScaleChange.Invoke(&*child);
}

void Transform::SetGlobalScale(const Vector3& scale)
{
	Matrix4 world = Matrix4::Translate(GetGlobalPosition()) * Quaternion::ToMatrix(m_worldRotation) * Matrix4::Scale(scale);

	if (m_parent)
		world = CCMaths::Matrix4::Inverse(m_parent->GetWorldMatrix()) * world;

	CCMaths::Vector3 tempPos;
	CCMaths::Vector3 tempRot;
	CCMaths::Vector3 tempScale;
	CCMaths::Matrix4::Decompose(world, tempPos, tempRot, tempScale);

	SetScale(tempScale);
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
	GetWorldMatrix();

	return m_worldPosition;
}

Quaternion Transform::GetGlobalRotation()
{
	GetWorldMatrix();
	
	return m_worldRotation;
}

Vector3 Transform::GetGlobalScale()
{
	GetWorldMatrix();
	
	return m_worldScale;
}

void Transform::Translate(const Vector3& translation)
{
	SetPosition(GetPosition() + translation);
}

void Transform::Rotate(const Vector3& eulerAngles)
{
	SetEuler(GetEuler() + eulerAngles);
}

void Transform::Rotate(const Vector3& axis, float angle)
{
	SetRotation(GetRotation() * Quaternion::FromAxisAngle(axis, angle));
}

void Transform::Copy(Behaviour* copy)
{
	Transform* copiedTransform = dynamic_cast<Transform*>(copy);

	SetPosition(copiedTransform->m_position);
	SetRotation(copiedTransform->m_rotation);
	SetScale(copiedTransform->m_scale);
	
	for (const auto& child : copiedTransform->m_children)
	{
		SceneManager::GetInstance()->m_currentScene->CopyEntity(child->GetHostPtr(), GetHostPtr());
	}
}
