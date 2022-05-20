#include "pch.hpp"

#include "camera.hpp"

using namespace CCMaths;

void Camera::UpdatePerspective()
{
    m_projectionMatrix = Matrix4::Perspective(m_fovY, m_aspect, m_near, m_far);
    m_frustumPlanes = FrustumPlanes::Create(m_viewMatrix, m_fovY, m_aspect, m_near, m_far);
}

void Camera::UpdateViewMatrix()
{
    m_viewMatrix = Matrix4::RotateXYZ(-m_rotation) * Matrix4::Translate(-m_position);
    m_frustumPlanes = FrustumPlanes::Create(m_viewMatrix, m_fovY, m_aspect, m_near, m_far);
}

void Camera::SetType(EType type)
{
    m_type = type;
    //TODO : Change perspective 
}

void Camera::SetAspect(float apsect)
{
    if (m_aspect == apsect)
        return;

    m_aspect = apsect;
    UpdatePerspective();
}

void Camera::SetNear(float near)
{
    if (m_near == near)
        return;

    m_near = near;
    UpdatePerspective();
}

void Camera::SetFar(float far)
{
    if (m_far == far)
        return;

    m_far = far;
    UpdatePerspective();
}

void Camera::SetFovY(float fovY)
{
    if (m_fovY == fovY)
        return;

    m_fovY = fovY;
    UpdatePerspective();
}

void Camera::SetWidth(float width)
{
    if (m_height == width)
        return;

    m_width = width;
    SetAspect(m_width / m_height);
    m_framebuffer->UpdateFramebuffer(m_width, m_height);
}

void Camera::SetHeight(float height)
{
    if (m_height == height)
        return;

    m_height = height;
    SetAspect(m_width / m_height);
    m_framebuffer->UpdateFramebuffer(m_width, m_height);
}

void Camera::SetSize(const CCMaths::Vector2& size)
{
    if (m_width == size.x && m_height == size.y)
        return;

    m_width = size.x;
    m_height = size.y;

    SetAspect(m_width / m_height);
    m_framebuffer->UpdateFramebuffer(m_width, m_height);
}

void Camera::SetPosition(const CCMaths::Vector3& position)
{
    SetLastPosition(m_position);
    m_position = position;
    UpdateViewMatrix();
}


void Camera::SetRotation(const CCMaths::Vector3& rotation)
{
    m_rotation = rotation;
    UpdateViewMatrix();
}

void Camera::SetPitch(float pitch)
{
    SetRotation({ pitch, m_rotation.yaw, m_rotation.roll });
}

void Camera::SetYaw(float yaw)
{
    SetRotation({ m_rotation.pitch, yaw, m_rotation.roll });
}

void Camera::SetRoll(float roll)
{
    SetRotation({ m_rotation.pitch, m_rotation.yaw, roll });
}

void Camera::SetLastPosition(const CCMaths::Vector3& lastPosition)
{
    m_lastPosition = lastPosition;
}

void Camera::SetUseFustrumCulling(bool useFrustrumCulling)
{
    m_useFustrumCulling = useFrustrumCulling;
}