#include "PrecompiledHeaders.h"
#include "objects/Camera.h"

#include "helpers/Time.h"

using namespace DirectX;

#pragma region Public Methods

Camera::Camera(Vector4 _position, Vector4 _up, float _yaw, float _pitch)
	: position(_position)
	, worldUp(_up)
	, yaw(_yaw)
	, pitch(_pitch)
	, front({0, 0, 1, 0})
{
	updateVectors();
}

const Matrix4 Camera::GetViewMatrix() const
{ 
	return XMMatrixLookAtLH(position, position + front, up);
}

const Matrix4 Camera::GetProjectionMatrix(uint32_t width, uint32_t height) const
{
	float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
	return XMMatrixPerspectiveFovLH(XMConvertToRadians(fov), aspectRatio, nearPlane, farPlane);
}

void Camera::ProcessInputs(Vector4 direction)
{
	float velocity = movementSpeed * Time::GetDeltaTime();
	position += front * velocity * direction + right * velocity * direction + up * velocity * direction;
}

#pragma endregion

#pragma region Private Methods

void Camera::updateVectors()
{
	XMVECTOR frontVector = XMVectorSet(
		cosf(XMConvertToRadians(yaw)) * cosf(XMConvertToRadians(pitch)),
		sinf(XMConvertToRadians(pitch)),
		sinf(XMConvertToRadians(yaw)) * cosf(XMConvertToRadians(pitch)),
		0.0f
	);

	XMVECTOR lerpFrontVector = XMVectorLerp(front, frontVector, Time::GetDeltaTime() * rotationSpeed);

	front = XMVector3Normalize(lerpFrontVector);
	right = XMVector3Normalize(XMVector3Cross(worldUp, front));
	up = XMVector3Normalize(XMVector3Cross(front, right));
}

#pragma endregion