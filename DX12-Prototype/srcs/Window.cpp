#include "PrecompiledHeaders.h"
#include "Window.h"

#include <iostream>

HWND Window::handleWin;
RECT Window::windowRect;
UINT Window::width;
UINT Window::height;
std::wstring Window::name;
std::unique_ptr<Editor> Window::editor;
bool Window::fullScreenState;

POINT Window::screenCenter;
bool Window::firstDrag;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#pragma region Public Methods

int Window::Run(HINSTANCE hInstance, UINT windowWidth, UINT windowHeight, std::wstring windowName, int cmdShow)
{
    width = windowWidth;
    height = windowHeight;
    name = windowName;
    firstDrag = true;

    editor = std::make_unique<Editor>(width, height);

    registerWindowClass(hInstance);
    createWindow(hInstance, L"DX12");

    // initialize the global window rect variable.
    ::GetWindowRect(handleWin, &windowRect);

    editor->OnInit(handleWin);

    ::ShowWindow(handleWin, cmdShow);

    // main sample loop.
    MSG msg = {};
    while (msg.message != WM_QUIT)
    {
        // process any messages in the queue.
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    editor->OnDestroy();

    // return this part of the WM_QUIT message to Windows.
    return static_cast<char>(msg.wParam);
}

#pragma endregion

#pragma region Private Methods

void Window::registerWindowClass(HINSTANCE hInst)
{
    // register a window class for creating our render window with.
    WNDCLASSEXW windowClass = {};

    windowClass.cbSize = sizeof(WNDCLASSEXW);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = WinProc;
    windowClass.hInstance = hInst;
    windowClass.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    windowClass.lpszClassName = name.c_str();

    static HRESULT hr = ::RegisterClassExW(&windowClass);
    assert(SUCCEEDED(hr));
}

void Window::createWindow(HINSTANCE hInst, const wchar_t* windowTitle)
{
    int screenWidth = ::GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = ::GetSystemMetrics(SM_CYSCREEN);

    RECT windowRect = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
    ::AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

    int windowWidth = windowRect.right - windowRect.left;
    int windowHeight = windowRect.bottom - windowRect.top;

    // center the window within the screen. Clamp to 0, 0 for the top-left corner.
    int windowX = std::max<int>(0, (screenWidth - windowWidth) / 2);
    int windowY = std::max<int>(0, (screenHeight - windowHeight) / 2);

    handleWin = ::CreateWindowExW(
        NULL,
        name.c_str(),
        windowTitle,
        WS_OVERLAPPEDWINDOW,
        windowX,
        windowY,
        windowWidth,
        windowHeight,
        NULL,
        NULL,
        hInst,
        editor.get()
    );

    assert(handleWin && "Failed to create window");
}

void Window::setFullScreen(bool fullScreen)
{
    if (fullScreenState != fullScreen)
    {
        fullScreenState = fullScreen;

        if (fullScreenState) // switching to fullscreen.
        {
            // store the current window dimensions so they can be restored 
            // when switching out of fullscreen state.
            ::GetWindowRect(handleWin, &windowRect);

            // set the window style to a borderless window so the client area fills
            // the entire screen.
            UINT windowStyle = WS_OVERLAPPEDWINDOW & ~(WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);

            ::SetWindowLongW(handleWin, GWL_STYLE, windowStyle);

            // query the name of the nearest display device for the window.
            // this is required to set the fullscreen dimensions of the window
            // when using a multi-monitor setup.
            HMONITOR hMonitor = ::MonitorFromWindow(handleWin, MONITOR_DEFAULTTONEAREST);
            MONITORINFOEX monitorInfo = {};
            monitorInfo.cbSize = sizeof(MONITORINFOEX);
            ::GetMonitorInfo(hMonitor, &monitorInfo);

            ::SetWindowPos(handleWin, HWND_NOTOPMOST,
                monitorInfo.rcMonitor.left,
                monitorInfo.rcMonitor.top,
                monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
                monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
                SWP_FRAMECHANGED | SWP_NOACTIVATE);

            ::ShowWindow(handleWin, SW_MAXIMIZE);
        }
        else
        {
            // restore all the window decorators.
            ::SetWindowLong(handleWin, GWL_STYLE, WS_OVERLAPPEDWINDOW);

            ::SetWindowPos(handleWin, HWND_NOTOPMOST,
                windowRect.left,
                windowRect.top,
                windowRect.right - windowRect.left,
                windowRect.bottom - windowRect.top,
                SWP_FRAMECHANGED | SWP_NOACTIVATE);

            ::ShowWindow(handleWin, SW_NORMAL);
        }
    }
}

void Window::resize()
{
    RECT clientRect = {};
    ::GetClientRect(handleWin, &clientRect);

    width = clientRect.right - clientRect.left;
    height = clientRect.bottom - clientRect.top;

    editor->Resize(width, height);
}

void Window::processInputs()
{
    float x = 0;
    float y = 0;
    float z = 0;
    bool accelerate = false;

    if (ImGui::IsKeyPressed(ImGuiKey_V, false))
        editor->ToggleVSync();

    if (ImGui::IsKeyPressed(ImGuiKey_F11, false))
        setFullScreen(!fullScreenState);

    if (ImGui::IsKeyDown(ImGuiKey_A)) x = -1;
    if (ImGui::IsKeyDown(ImGuiKey_D)) x = 1;

    if (ImGui::IsKeyDown(ImGuiKey_Q)) y = -1;
    if (ImGui::IsKeyDown(ImGuiKey_E)) y = 1;

    if (ImGui::IsKeyDown(ImGuiKey_W)) z = 1;
    if (ImGui::IsKeyDown(ImGuiKey_S)) z = -1;

    if (ImGui::IsKeyDown(ImGuiKey_LeftShift)) accelerate = true;

    editor->ProcessCameraInputs(x, y, z, accelerate);
}

void Window::processMouseDrag()
{
    // check if right click is currently pressed
    if (!GetAsyncKeyState(VK_RBUTTON))
    {
        if (!firstDrag)
        {
            firstDrag = true;
            ::SetCursorPos(screenCenter.x, screenCenter.y);
            ::SetCursor(LoadCursor(NULL, IDC_ARROW));
        }
        return;
    }

    if (firstDrag)
    {
        firstDrag = false;
        
        // center the cursor position
        screenCenter.x = static_cast<LONG>(windowRect.left + (windowRect.right - windowRect.left) * 0.5f);
        screenCenter.y = static_cast<LONG>(windowRect.top + (windowRect.bottom - windowRect.top) * 0.5f);
        ::SetCursorPos(screenCenter.x, screenCenter.y);
    }

    if (::GetCursor() != NULL) ::SetCursor(NULL);

    POINT cursorPos;
    ::GetCursorPos(&cursorPos);

    float xOffset = static_cast<float>(cursorPos.x - screenCenter.x);
    float yOffset = static_cast<float>(screenCenter.y - cursorPos.y); // reversed since y-coordinates go from bottom to top

    editor->ProcessCameraMouseMovement(xOffset, yOffset);
    ::SetCursorPos(screenCenter.x, screenCenter.y);
}

LRESULT CALLBACK Window::WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
        return true;

    Editor* editor = reinterpret_cast<Editor*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

    switch (uMsg)
    {
        case WM_CREATE:
        {
            LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
            SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
            return 0;
        }
        case WM_PAINT:
        {
            if (editor)
            {
                editor->OnUpdate();
                editor->OnRender();
                processInputs();
                if (!firstDrag) processMouseDrag();
            }
            return 0;
        }
        case WM_MOUSEMOVE:
        {
            if (editor && firstDrag)
                processMouseDrag();
            return 0;
        }
        case WM_MOUSEWHEEL:
        {
            if (editor)
                editor->ProcessCameraMouseScroll(GET_WHEEL_DELTA_WPARAM(wParam));
            return 0;
        }
        case WM_SIZE:
        {
            if (editor)
                resize();
            return 0;
        }
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }
    }

    // handle any messages the switch statement didn't.
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

#pragma endregion