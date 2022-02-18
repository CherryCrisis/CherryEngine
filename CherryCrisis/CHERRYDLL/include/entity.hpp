#pragma once

#include <memory>

#include "transform.hpp"

class Model;

class Entity
{
public:
	Entity();
	~Entity();

	//En attendant l'ECS !
	Transform*				m_transform;
	std::shared_ptr<Model>	m_model;
};