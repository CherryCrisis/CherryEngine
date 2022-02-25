#pragma once

#include "component.hpp"

#include "camera.hpp"
#include "transform.hpp"

class CameraComponent : public Behaviour
{
public:
	CameraComponent();
	~CameraComponent();

	Camera		m_camera;
	Transform*	m_transform = nullptr;
};