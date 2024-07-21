#include "PrecompiledHeaders.h"
#include "system/Editor.h"

#pragma region Public Methods

Editor::Editor(uint32_t _width, uint32_t _height)
	: width(_width)
	, height(_height)
{
	sample = std::make_unique<Sample>(width, height);
}

void Editor::OnInit(HWND hWnd)
{
	sample->OnInit(hWnd);
}

void Editor::OnUpdate()
{
	sample->OnUpdate();
	sample->OnRender();
}

void Editor::OnDestroy()
{
	sample->OnDestroy();
}

void Editor::ProcessCameraInputs(float x, float y, float z, bool accelerate)
{
    sample->ProcessCameraInputs(x, y, z, accelerate);
}

void Editor::ProcessCameraMouseMovement(float xOffset, float yOffset)
{
    sample->ProcessCameraMouseMovement(xOffset, yOffset);
}

void Editor::ProcessCameraMouseScroll(float offset)
{
    sample->ProcessCameraMouseScroll(offset);
}

void Editor::Resize(uint32_t _width, uint32_t _height)
{
    width = _width;
    height = _height;

    sample->Resize(width, height);
}

void Editor::ToggleVSync()
{
    sample->ToggleVSync();
}

#pragma endregion

#pragma region Private Methods

#pragma endregion