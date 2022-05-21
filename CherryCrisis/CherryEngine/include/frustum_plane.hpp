#pragma once

#include "maths.hpp"
#include "transform.hpp"
#include "mesh.hpp"

class Camera;

struct Plane
{
	CCMaths::Vector3 m_normal = { 0.f, 1.f, 0.f };

	// distance from origin to the nearest point in the plan
	float m_distance = 0.f;

	Plane() = default;

	Plane(const CCMaths::Vector3& p1, CCMaths::Vector3 normal)
		: m_normal(normal.Normalize()),
		m_distance(CCMaths::Vector3::Dot(normal, p1))
	{}

	float getSignedDistanceToPlan(const CCMaths::Vector3& point) const
	{
		return CCMaths::Vector3::Dot(m_normal, point) - m_distance;
	}
};

struct FrustumPlanes
{
	//-- https://learnopengl.com/Guest-Articles/2021/Scene/Frustum-Culling --//

    Plane topFace;
    Plane bottomFace;

    Plane rightFace;
    Plane leftFace;

    Plane farFace;
    Plane nearFace;

	//static FrustumPlanes Create(const CCMaths::Vector3& position, const CCMaths::Vector3& rotation, float fovY, float aspect, float near, float far);
	static FrustumPlanes Create(const CCMaths::Matrix4& viewMatrix, float fovY, float aspect, float near, float far);
	static FrustumPlanes CreateFromInverse(const CCMaths::Matrix4& viewMatrixInverse, float fovY, float aspect, float near, float far);

	//AABB Only !
	bool isOnOrForwardPlan(const Plane& plan, const AABB& aabb) const;

	//AABB Only !
	bool IsOnFrustum(const Matrix4& worldMatrix, const AABB& aabb) const;
};
