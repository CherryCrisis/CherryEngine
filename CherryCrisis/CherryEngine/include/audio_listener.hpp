#pragma once

#include <cherry_macros.hpp>

#include "behaviour.hpp"
#include "maths/vector3.hpp"

class Transform;


class CCENGINE_API AudioListener : public Behaviour
{
public:
	Transform* m_transform = nullptr; 

	AudioListener();
	AudioListener(CCUUID& id);
	~AudioListener();

	void InvalidateTransform();

	void Initialize();
	void BindToSignals() override;
	void UnbindSignals();

	void ChangePosition(Transform* transform);
	void ChangeRotation(Transform* transform);
};