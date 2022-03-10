#include "pch.hpp"

#include "entity.hpp"

Entity::Entity() 
{
	m_modelRenderer->m_transform = m_transform;
}

Entity::~Entity() {}

