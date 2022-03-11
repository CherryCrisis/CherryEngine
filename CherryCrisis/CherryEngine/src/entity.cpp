#include "pch.hpp"

#include "entity.hpp"

#include "behaviour.hpp"
#include "transform.hpp"

Entity::Entity(const std::string& name)
	: m_name(name)
{

}

Entity::~Entity()
{
	for (Behaviour* behaviour : m_behaviours)
		delete behaviour;
}

void Entity::Start()
{
	for (Behaviour* behaviour : m_behaviours)
		behaviour->Start();
}

void Entity::Update()
{
	for (Behaviour* behaviour : m_behaviours)
		behaviour->Update();
}

void Entity::Destroy()
{
	Transform* transform;
	if (!TryGetBehaviour(transform))
		return;

	std::vector<Transform*> children = transform->GetChildren();

	for (int i = 0; i < children.size(); i++)
		children[i]->GetHost().Destroy();
}
