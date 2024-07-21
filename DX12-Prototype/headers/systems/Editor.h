#pragma once

#include "objects/Camera.h"
#include "rendering/Sample.h"

// test
#include "objects/meshes/Mesh.h"

class Editor
{
public:
	Editor(uint32_t width, uint32_t height);

	void OnInit(HWND hWnd);
	void OnUpdate();
	void OnRender();
	void OnDestroy();

	// camera
	void ProcessCameraInputs(float x, float y, float z, bool accelerate);
	void ProcessCameraMouseMovement(float xOffset, float yOffset);
	void ProcessCameraMouseScroll(float offset);
	
	void Resize(uint32_t width, uint32_t height);
	void ToggleVSync();

private:
	void render();
	void setupRootSignature();

private:
	uint32_t width;
	uint32_t height;

	std::unique_ptr<Camera> camera;
	std::unique_ptr<Sample> sample;

	// test
	Mesh mesh;
	Mesh mesh2;
	Mesh mesh3;
};