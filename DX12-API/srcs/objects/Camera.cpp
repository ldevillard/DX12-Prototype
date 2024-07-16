#include "PrecompiledHeaders.h"
#include "objects/Camera.h"

#include <algorithm>

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

const float Camera::GetFOV() const
{
	return fov;
}

void Camera::ProcessInputs(float x, float y, float z, bool accelerate)
{
	float speedFactor = accelerate ? movementSpeedFactor : 1;
	float velocity = movementSpeed * Time::GetDeltaTime() * speedFactor;

	position += front * velocity * z + right * velocity * x + up * velocity * y;
}

void Camera::ProcessMouseMovement(float xOffset, float yOffset)
{
	float dt = Time::GetDeltaTime();

	yaw -= xOffset * rotationSpeedFactor;
	pitch -= yOffset * rotationSpeedFactor;

	pitch = std::clamp(pitch, -90.0f, 90.0f);

	updateVectors();
}

void Camera::ProcessMouseScroll(float offset)
{
	// 0.01f is an arbitrary value to slow down the scroll
	fov = std::clamp(fov - offset * 0.01f, 1.0f, 100.0f);
}

#pragma endregion

#pragma region Private Methods

void Camera::updateVectors()
{
	XMVECTOR frontVector = XMVectorSet(
		-cosf(XMConvertToRadians(yaw)) * cosf(XMConvertToRadians(pitch)),
		-sinf(XMConvertToRadians(pitch)),
		-sinf(XMConvertToRadians(yaw)) * cosf(XMConvertToRadians(pitch)),
		0.0f
	);

	XMVECTOR lerpFrontVector = XMVectorLerp(front, frontVector, Time::GetDeltaTime() * rotationSpeed);

	front = XMVector3Normalize(lerpFrontVector);
	right = XMVector3Normalize(XMVector3Cross(worldUp, front));
	up = XMVector3Normalize(XMVector3Cross(front, right));
}

#pragma endregion