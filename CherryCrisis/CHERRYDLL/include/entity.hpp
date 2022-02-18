#pragma once

#include <memory>

#include <CherryHeader.h>

#include "transform.hpp"
#include "model_renderer.hpp"

class CCENGINE_API Entity
{
public:
	Entity();
	~Entity();

	//En attendant l'ECS !
	Transform*		m_transform = new Transform();
	ModelRenderer*  m_modelRenderer = new ModelRenderer();
};
