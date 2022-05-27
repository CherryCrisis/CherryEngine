#pragma once

#include <cherry_macros.hpp>

#include "behaviour.hpp"
#include "light.hpp"

class Cell;
class Transform;


class CCENGINE_API LightComponent : public Behaviour, public ARenderer
{
protected:
	void PopulateMetadatas() override;

public:
	Light		m_light;

	Transform*	m_transform = nullptr;

	LightComponent();
	LightComponent(CCUUID& id);
	~LightComponent();

	void Initialize();
	void BindToSignals() override;

	void ChangePosition(Transform* transform);
	void ChangeRotation(Transform* transform);

	void SubscribeToPipeline(ARenderingPipeline* pipeline) override;
	void UnsubscribeToPipeline(ARenderingPipeline* pipeline) override;
	void OnCellAdded(Cell* newCell);
	void OnCellRemoved(Cell* newCell);

	void SubscribeGuizmo();
	void UnsubscribeGuizmo();

	void InvalidateTransform();
};