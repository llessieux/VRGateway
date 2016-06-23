////////////////////////////////////////////////////////////////////////////////
// Filename: cameraclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "camera.h"
#include <DirectXMath.h>

Camera::Camera()
{
    m_position.set(0.0f,0.0f,0.0f);
    m_rotation.set(0.0f, 0.0f, 0.0f);
}

Camera::~Camera()
{
}

void Camera::SetPosition(float x, float y, float z)
{
    m_position.set(x, y, z); 
}


void Camera::SetRotation(float x, float y, float z)
{
    m_rotation.set(x,y,z);
}


D3DXVECTOR3 Camera::GetPosition()
{
    return m_position;
}


D3DXVECTOR3 Camera::GetRotation()
{
    return m_rotation;
}


void Camera::Render()
{
    // Setup the vector that points upwards.
    D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);

    // Setup where the camera is looking by default.
    D3DXVECTOR3 lookAt(0.0f, 0.0f, 1.0f);

    D3DXMATRIX rotationMatrix;

    Vector3 rt = m_rotation * 0.0174532925f;

    // Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
    float pitch = rt.x;
    float yaw   = rt.y;
    float roll  = rt.z;

    // Create the rotation matrix from the yaw, pitch, and roll values.
    //D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, pitch, roll);
    //rotationMatrix.rotateY(pitch);
    //rotationMatrix.rotateX(yaw);
    //rotationMatrix.rotateZ(roll);
    DirectX::XMMATRIX temp = DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
    rotationMatrix.set(reinterpret_cast<const float*>(&temp));

    // Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
    //D3DXVec3TransformCoord(&lookAt, &lookAt, &rotationMatrix);
    //D3DXVec3TransformCoord(&up, &up, &rotationMatrix);
    lookAt = rotationMatrix * lookAt;
    up = rotationMatrix * up;

    // Translate the rotated camera position to the location of the viewer.
    lookAt = m_position + lookAt;

    // Finally create the view matrix from the three updated vectors.
    //D3DXMatrixLookAtLH(&m_viewMatrix, &position, &lookAt, &up); // implemented below


    Vector3 RightVector(1.0f, 0, 0);//D3DXVector3(1.0, 0.0, 0.0);

    //D3DXVec3Subtract(ZAxis, LookAtPointVector, EyePointVector);
    //D3DXVec3Normalize(ZAxis, ZAxis);
    Vector3 ZAxis = lookAt - m_position;
    ZAxis.normalize();

    //D3DXVec3Cross(XAxis, UpVector, ZAxis);
    //D3DXVec3Normalize(XAxis, XAxis);
    Vector3 XAxis = up.cross(ZAxis);
    XAxis.normalize();

    //D3DXVec3Cross(YAxis, ZAxis, XAxis);
    Vector3 YAxis = ZAxis.cross(XAxis);
    YAxis.normalize();

    //viewMatrix._11 : = XAxis.x; viewMatrix._12 : = YAxis.x; viewMatrix._13 : = ZAxis.x; viewMatrix._14 : = 0.0;
    //viewMatrix._21 : = XAxis.y; viewMatrix._22 : = YAxis.y; viewMatrix._23 : = ZAxis.y; viewMatrix._24 : = 0.0;
    //viewMatrix._31 : = XAxis.z; viewMatrix._32 : = YAxis.z; viewMatrix._33 : = ZAxis.z; viewMatrix._34 : = 0.0;

    //viewMatrix._41 : = -D3DXVec3Dot(XAxis, EyePointVector);
    //viewMatrix._42 : = -D3DXVec3Dot(YAxis, EyePointVector);
    //viewMatrix._43 : = -D3DXVec3Dot(ZAxis, EyePointVector);
    //viewMatrix._44 : = 1.0;

    m_viewMatrix.set(XAxis.x, YAxis.x, ZAxis.x, 0.0f,
        XAxis.y, YAxis.y, ZAxis.y, 0.0f,
        XAxis.z, YAxis.z, ZAxis.z, 0.0f,
        -XAxis.dot(m_position), -YAxis.dot(m_position), -ZAxis.dot(m_position), 1.0f);

    //m_viewMatrix.transpose();
}


void Camera::GetViewMatrix(D3DXMATRIX& viewMatrix)
{
    viewMatrix = m_viewMatrix;
}