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
	// TODO: REMOVE THIS
	std::shared_ptr<ModelRenderer>  m_modelRenderer = std::make_shared<ModelRenderer>();
};
