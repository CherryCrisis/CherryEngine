#pragma once

#include "behaviour.hpp"

#include "cherry_macros.hpp"
#include "maths/vector3.hpp"

class Transform;

class CCENGINE_API AudioListener : public Behaviour
{
public:
	AudioListener();
	AudioListener(CCUUID& id);
	~AudioListener();

	Transform* m_transform = nullptr; 

	void Initialize();
	void BindToSignals() override;
	
	void ChangePosition(const CCMaths::Vector3& position);
	void ChangeRotation(const CCMaths::Vector3& position);
};