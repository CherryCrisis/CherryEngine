#include "pch.hpp"

#include "frustum_plane.hpp"
#include "camera.hpp"

using namespace CCMaths;

FrustumPlanes FrustumPlanes::Create(const Camera& camera, float fovY, float aspect, float near, float far)
{
    FrustumPlanes frustumPlanes;

    Vector3 position = camera.GetPosition();
    Vector3 rotation = camera.GetRotation();
    CCMaths::Matrix4 view = Matrix4::RotateYXZ(rotation);

    Vector3 up = Vector3::Up;
    Vector3 right = view.right;
    Vector3 forward = -view.back;

    const float halfVSide = far * CCMaths::Tan(fovY * 0.5f);
    const float halfHSide = halfVSide * aspect;
    const CCMaths::Vector3 frontMultFar = forward * far;

    frustumPlanes.nearFace = { position + forward * near, forward};
    frustumPlanes.farFace = { position + frontMultFar, -forward };
    frustumPlanes.rightFace = { position, Vector3::Cross(up, frontMultFar + right * halfHSide) };
    frustumPlanes.leftFace = { position, Vector3::Cross(frontMultFar - right * halfHSide, up) };
    frustumPlanes.topFace = { position, Vector3::Cross(right, frontMultFar - up * halfVSide) };
    frustumPlanes.bottomFace = { position, Vector3::Cross(frontMultFar + up * halfVSide, right) };

	return frustumPlanes;
}

bool FrustumPlanes::isOnOrForwardPlan(const Plane& plan, const AABB& aabb) const
{
	// Compute the projection interval radius of b onto L(t) = b.c + t * p.n
	const float r = aabb.m_extents.x * CCMaths::Abs(plan.m_normal.x) + aabb.m_extents.y * CCMaths::Abs(plan.m_normal.y) +
		aabb.m_extents.z * CCMaths::Abs(plan.m_normal.z);

	return -r <= plan.getSignedDistanceToPlan(aabb.m_center);
}

bool FrustumPlanes::IsOnFrustum(Transform& transform, const AABB& aabb) const
{
	Matrix4 worldMatrix = transform.GetWorldMatrix();

	//Get global scale thanks to our transform
	const Vector3 globalCenter((worldMatrix * Vector4(aabb.m_center, 1.f) ).xyz);

	// Scaled orientation
	const Vector3 right = worldMatrix.right * aabb.m_extents.x;
	const Vector3 up = worldMatrix.up * aabb.m_extents.y;
	const Vector3 forward = (-worldMatrix.back) * aabb.m_extents.z;

	const float newIi = Abs(Vector3::Dot(Vector3{ 1.f, 0.f, 0.f }, right)) +
		Abs(Vector3::Dot(Vector3{ 1.f, 0.f, 0.f }, up)) +
		Abs(Vector3::Dot(Vector3{ 1.f, 0.f, 0.f }, forward));

	const float newIj = Abs(Vector3::Dot(Vector3{ 0.f, 1.f, 0.f }, right)) +
		Abs(Vector3::Dot(Vector3{ 0.f, 1.f, 0.f }, up)) +
		Abs(Vector3::Dot(Vector3{ 0.f, 1.f, 0.f }, forward));

	const float newIk = Abs(Vector3::Dot(Vector3{ 0.f, 0.f, 1.f }, right)) +
		Abs(Vector3::Dot(Vector3{ 0.f, 0.f, 1.f }, up)) +
		Abs(Vector3::Dot(Vector3{ 0.f, 0.f, 1.f }, forward));

	const AABB globalAABB(globalCenter, newIi, newIj, newIk);

	return (isOnOrForwardPlan(leftFace, globalAABB) &&
		isOnOrForwardPlan(rightFace, globalAABB) &&
		isOnOrForwardPlan(topFace, globalAABB) &&
		isOnOrForwardPlan(bottomFace, globalAABB) &&
		isOnOrForwardPlan(nearFace, globalAABB) &&
		isOnOrForwardPlan(farFace, globalAABB));
};