#pragma once

#include "helpers/Helpers.h"
#include "rendering/Sample.h"

class Window
{
public:
	static int Run(HINSTANCE hInstance, UINT width, UINT height, std::wstring name, int cmdShow);

private:
	static void registerWindowClass(HINSTANCE hInst);
	static void createWindow(HINSTANCE hInst, const wchar_t* windowTitle);

	static LRESULT CALLBACK WinProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	static HWND handleWin;
	static RECT windowRect;
	static UINT width;
	static UINT heigth;
	static std::wstring name;

	static std::unique_ptr<Sample> sample;
};