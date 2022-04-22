#pragma once

#include "behaviour.hpp"

#include "camera.hpp"
#include "maths.hpp"

class Transform;

class CameraComponent : public Behaviour
{
protected:
	void PopulateMetadatas() override;

public:
	Camera		m_camera;
	Transform*	m_transform = nullptr;

	CameraComponent();
	CameraComponent(CCUUID& id);
	~CameraComponent();

	void Initialize();
	void BindToSignals() override;

	void ChangePosition(const CCMaths::Vector3& position);
	void ChangeRotation(const CCMaths::Vector3& rotation);

	void UpdateCameraModel();
};