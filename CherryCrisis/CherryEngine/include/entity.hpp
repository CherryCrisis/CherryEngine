#pragma once

#include <memory>

#include "cherry_header.hpp"

#include "transform.hpp"
#include "model_renderer.hpp"
#include "light_component.hpp"
#include "camera_component.hpp"

class CCENGINE_API Entity
{
public:
	Entity();
	~Entity();

	//En attendant l'ECS !
	// TODO: systeme de components
	Transform*				m_transform = new Transform();
	CameraComponent*		m_cameraComp = nullptr;
	LightComponent*			m_lightComp = nullptr;
	// TODO: REMOVE THIS
	std::shared_ptr<ModelRenderer>  m_modelRenderer = std::make_shared<ModelRenderer>();
};
