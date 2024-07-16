#pragma once

#include "maths/Maths.h"

class Camera
{
public:
	Camera(Vector4 position = { 0.0f, 0.0f, 0.0f, 1.0f }, Vector4 up = { 0.0f, 1.0f, 0.0f }, float yaw = -90.0f, float pitch = 0.0f);

	const Matrix4 GetViewMatrix() const;
	const Matrix4 GetProjectionMatrix(uint32_t width, uint32_t height) const;

	void ProcessInputs(float x, float y, float z, bool accelerate);
	void ProcessMouseMovement(float xOffset, float yOffset);

private:
	void updateVectors();

private:
	// settings
	float movementSpeed = 10.f;
	float movementSpeedFactor = 2.0f;
	float rotationSpeed = 15.f;
	float rotationSpeedFactor = 0.1f;
	float fov = 45.0;
	float nearPlane = 0.1f;
	float farPlane = 1000.f;
	float yaw = -90.0f;
	float pitch = 0.0f;

	Vector4 position;
	Vector4 front;
	Vector4 up;
	Vector4 right;
	Vector4 worldUp;
	
	Matrix4 viewMatrix;
	Matrix4 projectionMatrix;
};