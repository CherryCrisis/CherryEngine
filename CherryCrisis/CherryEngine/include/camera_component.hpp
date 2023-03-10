#pragma once

#include <cherry_header.hpp>
#include <cherry_macros.hpp>

#include "behaviour.hpp"
#include "camera.hpp"
#include "maths.hpp"

class Transform;


class CCENGINE_API CameraComponent : public Behaviour, public ARenderer
{
private:
	static unsigned int		m_count;

	static CameraComponent* m_mainCamera;

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

	void ChangePosition(Transform* transform);
	void ChangeRotation(Transform* transform);

	static CameraComponent* GetMainCamera();

	void SetAsMain();
	void InvalidateTransform();

	void OnCellAdded(Cell* newCell);
	void OnCellRemoved(Cell* newCell);

	void SubscribeToPipeline(ARenderingPipeline* pipeline) override;
	void UnsubscribeToPipeline(ARenderingPipeline* pipeline) override;
	
	static Camera* m_editorCamera;
};