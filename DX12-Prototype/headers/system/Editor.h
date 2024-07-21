#pragma once

#include "rendering/Sample.h"

class Editor
{
public:
	Editor(uint32_t width, uint32_t height);

	// states
	void OnInit(HWND hWnd);
	void OnUpdate();
	void OnDestroy();

	// camera
	void ProcessCameraInputs(float x, float y, float z, bool accelerate);
	void ProcessCameraMouseMovement(float xOffset, float yOffset);
	void ProcessCameraMouseScroll(float offset);
	
	void Resize(uint32_t width, uint32_t height);
	void ToggleVSync();

private:
	uint32_t width;
	uint32_t height;

	std::unique_ptr<Sample> sample;
};