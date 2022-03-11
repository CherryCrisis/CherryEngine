#pragma once

#include "behaviour.hpp"

#include "camera.hpp"
#include "transform.hpp"

class CameraComponent : public Behaviour
{
public:
	CameraComponent(Entity& owner);
	~CameraComponent();

	Camera		m_camera;
	Transform*	m_transform = nullptr;
};