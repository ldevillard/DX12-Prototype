#pragma once

#include "helpers/Helpers.h"
#include "systems/Editor.h"

class Window
{
public:
	static int Run(HINSTANCE hInstance, UINT width, UINT height, std::wstring name, int cmdShow);

private:
	static void registerWindowClass(HINSTANCE hInst);
	static void createWindow(HINSTANCE hInst, const wchar_t* windowTitle);
	static void setFullScreen(bool fullScreen);
	static void resize();

	static void processInputs();
	static void processMouseDrag();

	static LRESULT CALLBACK WinProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	static uint32_t width;
	static uint32_t height;
	static HWND handleWin;
	static RECT windowRect;
	static std::wstring name;
	static bool fullScreenState;

	static std::unique_ptr<Editor> editor;

	// mouse settings, TODO: Need input class
	static POINT screenCenter;
	static bool firstDrag;
};