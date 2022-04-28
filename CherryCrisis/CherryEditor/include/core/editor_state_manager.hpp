#pragma once

#include <vector>

#include "maths/vector3.hpp"
#include "singleton.hpp"

class Transform;

struct Action 
{
	Action(Transform* owner);

	Transform* m_owner;
	CCMaths::Vector3 m_position;
	CCMaths::Vector3 m_scale;
	CCMaths::Vector3 m_rotation;

	void Apply();
};

class EditorStateManager : public Singleton<EditorStateManager>
{
	static void RegisterAction(Transform* transform);

	std::vector<Action> m_actions;

	static void UndoAction();
};