#include "PrecompiledHeaders.h"

#include "Window.h"

_Use_decl_annotations_
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
	SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
	
	Window::Run(hInstance, 1280, 720, L"DX12 - Prototype", nCmdShow);

	return 0;
}