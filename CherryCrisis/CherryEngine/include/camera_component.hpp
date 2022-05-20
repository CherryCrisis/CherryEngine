#pragma once

#include "behaviour.hpp"

#include "cherry_macros.hpp"

#include "camera.hpp"
#include "maths.hpp"

#include <cherry_header.hpp>

class Transform;

class CCENGINE_API CameraComponent : public Behaviour, public ARenderer
{
private:
	static unsigned int m_count;
	static CameraComponent* m_mainCamera;

protected:
	void PopulateMetadatas() override;

public:
	static Camera* m_editorCamera;
	
	Camera		m_camera;
	Transform*	m_transform = nullptr;

	CameraComponent();
	CameraComponent(CCUUID& id);
	~CameraComponent();

	void Initialize();
	void BindToSignals() override;

	void ChangePosition(const CCMaths::Vector3& position);
	void ChangeRotation(const CCMaths::Vector3& rotation);

	static CameraComponent* GetMainCamera();

	void SetAsMain();
	void InvalidateTransform();

	void SubscribeToPipeline(ARenderingPipeline* pipeline) override;
	void UnsubscribeToPipeline(ARenderingPipeline* pipeline) override;
};