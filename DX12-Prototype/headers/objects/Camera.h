#pragma once

#include "maths/Maths.h"

class Camera
{
public:
	Camera(Vector position = { 0.0f, 0.0f, 0.0f, 1.0f }, Vector up = { 0.0f, 1.0f, 0.0f }, float yaw = -90.0f, float pitch = 0.0f);

	const Matrix4& GetViewMatrix() const;
	const Matrix4& GetProjectionMatrix(uint32_t width, uint32_t height) const;
	const float GetFOV() const;
	const Vector& GetPosition() const;

	void OnUpdate(uint32_t width, uint32_t height);

	void ProcessInputs(float x, float y, float z, bool accelerate);
	void ProcessMouseMovement(float xOffset, float yOffset);
	void ProcessMouseScroll(float offset);

private:
	void updateVectors();

private:
	float movementSpeed = 5.f;
	float movementSpeedFactor = 2.0f;
	float rotationSpeed = 10.f;
	float rotationSpeedFactor = 0.1f;

	float nearPlane = 0.1f;
	float farPlane = 1000.f;

	float fov = 45.0;
	float yaw = -90.0f;
	float pitch = 0.0f;

	Vector position;
	Vector front;
	Vector up;
	Vector right;
	Vector worldUp;

	Matrix4 viewMatrix;
	Matrix4 projectionMatrix;
};