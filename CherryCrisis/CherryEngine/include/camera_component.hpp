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

	void ChangePosition(const Vector3& position);
	void ChangeRotation(const Vector3& rotation);

	Camera		m_camera;
	Transform*	m_transform = nullptr;
};