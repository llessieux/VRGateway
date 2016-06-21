////////////////////////////////////////////////////////////////////////////////
// Filename: cameraclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "camera.h"
#include <DirectXMath.h>

Camera::Camera()
{
	m_positionX = 0.0f;
	m_positionY = 0.0f;
	m_positionZ = 0.0f;

	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;
}


Camera::Camera(const Camera& other)
{
}


Camera::~Camera()
{
}


void Camera::SetPosition(float x, float y, float z)
{
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;
	return;
}


void Camera::SetRotation(float x, float y, float z)
{
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;
	return;
}


D3DXVECTOR3 Camera::GetPosition()
{
	return D3DXVECTOR3(m_positionX, m_positionY, m_positionZ);
}


D3DXVECTOR3 Camera::GetRotation()
{
	return D3DXVECTOR3(m_rotationX, m_rotationY, m_rotationZ);
}


void Camera::Render()
{
	D3DXVECTOR3 up, position, lookAt;
	float yaw, pitch, roll;
	D3DXMATRIX rotationMatrix;

	// Setup the vector that points upwards.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// Setup the position of the camera in the world.
	position.x = m_positionX;
	position.y = m_positionY;
	position.z = m_positionZ;

	// Setup where the camera is looking by default.
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;




	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	pitch = m_rotationX * 0.0174532925f;
	yaw   = m_rotationY * 0.0174532925f;
	roll  = m_rotationZ * 0.0174532925f;

	// Create the rotation matrix from the yaw, pitch, and roll values.
	//D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, pitch, roll);
	//rotationMatrix.rotateY(pitch);
	//rotationMatrix.rotateX(yaw);
	//rotationMatrix.rotateZ(roll);
	DirectX::XMMATRIX temp = DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
	rotationMatrix.set((const float*)temp.r);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	//D3DXVec3TransformCoord(&lookAt, &lookAt, &rotationMatrix);
	//D3DXVec3TransformCoord(&up, &up, &rotationMatrix);
	lookAt = rotationMatrix * lookAt;
	up = rotationMatrix * up;

	// Translate the rotated camera position to the location of the viewer.
	lookAt = position + lookAt;

	// Finally create the view matrix from the three updated vectors.
	//D3DXMatrixLookAtLH(&m_viewMatrix, &position, &lookAt, &up); // implemented below


	Vector3 RightVector = Vector3(1.0f, 0, 0);//D3DXVector3(1.0, 0.0, 0.0);

	//D3DXVec3Subtract(ZAxis, LookAtPointVector, EyePointVector);
	//D3DXVec3Normalize(ZAxis, ZAxis);
	Vector3 ZAxis = lookAt - position;
	ZAxis.normalize();

	//D3DXVec3Cross(XAxis, UpVector, ZAxis);
	//D3DXVec3Normalize(XAxis, XAxis);
	Vector3 XAxis = up.cross(ZAxis);

	//D3DXVec3Cross(YAxis, ZAxis, XAxis);
	Vector3 YAxis = ZAxis.cross(XAxis);

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
		-XAxis.dot(position), -YAxis.dot(position), -ZAxis.dot(position), 1.0f);

	//m_viewMatrix.transpose();

	return;
}


void Camera::GetViewMatrix(D3DXMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
	return;
}