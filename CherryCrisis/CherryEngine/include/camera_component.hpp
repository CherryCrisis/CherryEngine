#pragma once

#include "behaviour.hpp"

#include "camera.hpp"
#include "transform.hpp"

class CameraComponent : public Behaviour
{
protected:
	void PopulateMetadatas() override;

public:
	CameraComponent(Entity& owner);
	CameraComponent();
	~CameraComponent();

	Camera		m_camera;
	Transform*	m_transform = nullptr;
};