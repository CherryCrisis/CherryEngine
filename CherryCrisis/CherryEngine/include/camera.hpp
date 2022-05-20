#pragma once

#include "maths.hpp"

#include "viewer.hpp"
#include "framebuffer.hpp"
#include "event.hpp"

class CCENGINE_API Camera : public Viewer
{
private:
	enum class EType
	{
		PERSPECTIVE,
		ORTHOGRAPHIC
	};

	EType m_type = EType::PERSPECTIVE;

	float m_aspect = 4.f / 3.f;
	float m_near = 0.01f;
	float m_far = 1500.f;
	float m_fovY = CCMaths::PI / 3.f;
	float m_width = 1920.f;
	float m_height = 1080.f;

	CCMaths::Vector3 m_rotation;
	CCMaths::Vector3 m_lastPosition;

	bool m_isEnabled = true;
	bool m_useFustrumCulling = true;

	void UpdatePerspective();
	void UpdateViewMatrix();

public :
	~Camera() {};

	//To serialize
	const float* GetAspectPtr() { return &m_aspect; }
	const float* GetNearPtr() { return &m_near; }
	const float* GetFarPtr() { return &m_far; }
	const float* GetFovYPtr() { return &m_fovY; }

	EType GetType() const { return m_type; }
	void SetType(EType type);

	float GetAspect() const { return m_aspect; }
	void SetAspect(float apsect);

	float GetNear() const { return m_near; }
	void SetNear(float near);

	float GetFar() const { return m_far; }
	void SetFar(float far);

	float GetFovY() const { return m_fovY; }
	void SetFovY(float fovY);

	float GetWidth() const { return m_width; }
	void SetWidth(float width);

	float GetHeight() const { return m_height; }
	void SetHeight(float height);

	CCMaths::Vector2 GetSize() const { return { m_width, m_height }; }
	void SetSize(const CCMaths::Vector2& size);

	CCMaths::Vector3 GetPosition() const { return m_position; }
	void SetPosition(const CCMaths::Vector3& position);

	void SetPitch(float pitch);
	void SetYaw(float yaw);
	void SetRoll(float roll);

	CCMaths::Vector3 GetRotation() const { return m_rotation; }
	void SetRotation(const CCMaths::Vector3& rotation);

	CCMaths::Vector3 GetLastPosition() const { return m_lastPosition; }
	void SetLastPosition(const CCMaths::Vector3& lastPosition);

	bool GetIsEnabled() const { return m_isEnabled; }
	void SetIsEnabled(bool isEnabled) { m_isEnabled = isEnabled; }

	bool GetUseFustrumCulling() const { return m_useFustrumCulling; }
	void SetUseFustrumCulling(bool useFrustrumCulling);
};