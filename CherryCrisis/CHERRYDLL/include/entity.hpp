#pragma once

#include <memory>

#include "transform.hpp"
#include "model_renderer.hpp"

class Entity
{
public:
	Entity();
	~Entity();

	//En attendant l'ECS !
	Transform*		m_transform = new Transform();
	ModelRenderer*	m_modelRenderer;
};