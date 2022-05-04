
#include "core/editor_state_manager.hpp"

#include "transform.hpp"

template <>
EditorStateManager* Singleton<EditorStateManager>::currentInstance = nullptr;

Action::Action(Transform* owner) :
	m_owner(owner), m_position(owner->GetPosition()), m_rotation(owner->GetRotation()), m_scale(owner->GetScale()) {}

void Action::Apply()
{
	m_owner->SetPosition(m_position);
	m_owner->SetRotation(m_rotation);
	m_owner->SetScale(m_scale);
}

void EditorStateManager::RegisterAction(Transform* transform)
{
	Action action{transform};
	GetInstance()->m_actions.push_back(action);
}

void EditorStateManager::UndoAction() 
{
	GetInstance()->m_actions.back().Apply();
	GetInstance()->m_actions.pop_back();
}