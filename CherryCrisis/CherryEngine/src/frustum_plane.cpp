#include <pch.hpp>

#include "frustum_plane.hpp"

#include "camera.hpp"

using namespace CCMaths;


FrustumPlanes FrustumPlanes::Create(const CCMaths::Matrix4& viewMatrix, float fovY, float aspect, float near, float far)
{
    CCMaths::Matrix4 viewMatrixInverse = viewMatrix.Inverse();
	return CreateFromInverse(viewMatrixInverse, fovY, aspect, near, far);
}

FrustumPlanes FrustumPlanes::CreateFromInverse(const CCMaths::Matrix4& viewMatrixInverse, float fovY, float aspect, float near, float far)
{
	FrustumPlanes frustumPlanes;

	Vector3 up = viewMatrixInverse.up.Normalized();
	Vector3 right = viewMatrixInverse.right.Normalized();
	Vector3 forward = -viewMatrixInverse.back.Normalized();

	const float halfVSide = far * CCMaths::Tan(fovY * 0.5f);
	const float halfHSide = halfVSide * aspect;
	const CCMaths::Vector3 frontMultFar = forward * far;

	frustumPlanes.nearFace = { viewMatrixInverse.position + forward * near, forward };
	frustumPlanes.farFace = { viewMatrixInverse.position + frontMultFar, -forward };
	frustumPlanes.rightFace = { viewMatrixInverse.position, Vector3::Cross(up, frontMultFar + right * halfHSide) };
	frustumPlanes.leftFace = { viewMatrixInverse.position, Vector3::Cross(frontMultFar - right * halfHSide, up) };
	frustumPlanes.topFace = { viewMatrixInverse.position, Vector3::Cross(right, frontMultFar - up * halfVSide) };
	frustumPlanes.bottomFace = { viewMatrixInverse.position, Vector3::Cross(frontMultFar + up * halfVSide, right) };

	return frustumPlanes;
}

bool FrustumPlanes::isOnOrForwardPlan(const Plane& plan, const AABB& aabb) const
{
	// Compute the projection interval radius of b onto L(t) = b.c + t * p.n
	const float r = aabb.m_extents.x * CCMaths::Abs(plan.m_normal.x) + aabb.m_extents.y * CCMaths::Abs(plan.m_normal.y) +
		aabb.m_extents.z * CCMaths::Abs(plan.m_normal.z);

	return -r <= plan.getSignedDistanceToPlan(aabb.m_center);
}

bool FrustumPlanes::IsOnFrustum(const Matrix4& worldMatrix, const AABB& aabb) const
{
	//Get global scale thanks to our transform
	const Vector3 globalCenter((worldMatrix * Matrix4::Translate(aabb.m_center)).position);

	// Scaled orientation
	const Vector3 right = worldMatrix.right * aabb.m_extents.x;
	const Vector3 up = worldMatrix.up * aabb.m_extents.y;
	const Vector3 forward = (-worldMatrix.back) * aabb.m_extents.z;

	const float newIi = Abs(Vector3::Dot(Vector3::Right, right)) +
		Abs(Vector3::Dot(Vector3::Right, up)) +
		Abs(Vector3::Dot(Vector3::Right, forward));

	const float newIj = Abs(Vector3::Dot(Vector3::Up, right)) +
		Abs(Vector3::Dot(Vector3::Up, up)) +
		Abs(Vector3::Dot(Vector3::Up, forward));

	const float newIk = Abs(Vector3::Dot(Vector3::Forward, right)) +
		Abs(Vector3::Dot(Vector3::Forward, up)) +
		Abs(Vector3::Dot(Vector3::Forward, forward));

	const AABB globalAABB(globalCenter, newIi, newIj, newIk);

	return (isOnOrForwardPlan(leftFace, globalAABB) &&
		isOnOrForwardPlan(rightFace, globalAABB) &&
		isOnOrForwardPlan(topFace, globalAABB) &&
		isOnOrForwardPlan(bottomFace, globalAABB) &&
		isOnOrForwardPlan(nearFace, globalAABB) &&
		isOnOrForwardPlan(farFace, globalAABB));
};