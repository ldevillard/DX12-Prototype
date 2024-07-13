#include "PrecompiledHeaders.h"
#include "rendering/Sample.h"

#include <chrono>

#include "resources/Resource.h"

#pragma region Public Methods

Sample::Sample(uint32_t w, uint32_t h)
	: width(w)
	, heigth(h)
    , timer()
{
    parseCommandLineArguments();
    enableDebugLayer();
	allowTearing = checkTearingSupport();
}

void Sample::OnInit(HWND hWnd)
{
    handleWin = hWnd;
	loadPipeline();
}

void Sample::OnUpdate()
{
    timer.OnUpdate();

    std::string fps = std::to_string(timer.GetFrameRate()) + "\n";
    OutputDebugStringA(fps.c_str());
}

void Sample::OnRender()
{
    UINT currentBackBufferIndex = swapChain->GetCurrentBackBufferIndex();
    std::unique_ptr<CommandAllocator>& commandAllocator = commandAllocators[currentBackBufferIndex];
    Resource backBuffer = swapChain->GetCurrentBackBuffer();

    commandList->Populate(*RTVdescriptorHeap, *commandAllocator, backBuffer, currentBackBufferIndex);

    ID3D12CommandList* const commandLists[] = 
    {
        commandList->GetPtr()
    };
    commandQueue->Get()->ExecuteCommandLists(_countof(commandLists), commandLists);
    
    frameFenceValues[currentBackBufferIndex] = fence->Signal(*commandQueue);
    
    UINT syncInterval = vSync ? 1 : 0;
    UINT presentFlags = allowTearing && !vSync ? DXGI_PRESENT_ALLOW_TEARING : 0;
    ThrowIfFailed(swapChain->Get()->Present(syncInterval, presentFlags));
    
    currentBackBufferIndex = swapChain->GetCurrentBackBufferIndex();
    
    fence->WaitForFenceValue(frameFenceValues[currentBackBufferIndex]);
}

void Sample::OnDestroy()
{
    fence->Flush(*commandQueue);
    CloseHandle(fence->GetEvent());
}

void Sample::ToggleVSync()
{
    vSync = !vSync;
}

#pragma endregion

#pragma region Private Methods

bool Sample::checkTearingSupport()
{
    BOOL allowTearing = FALSE;

    // rather than create the DXGI 1.5 factory interface directly, we create the
    // DXGI 1.4 interface and query for the 1.5 interface. This is to enable the 
    // graphics debugging tools which will not support the 1.5 factory interface 
    // until a future update.
    ComPtr<IDXGIFactory4> factory4;
    if (SUCCEEDED(CreateDXGIFactory1(IID_PPV_ARGS(&factory4))))
    {
        ComPtr<IDXGIFactory5> factory5;
        if (SUCCEEDED(factory4.As(&factory5)))
        {
            if (FAILED(factory5->CheckFeatureSupport(
                DXGI_FEATURE_PRESENT_ALLOW_TEARING,
                &allowTearing, sizeof(allowTearing))))
            {
                allowTearing = FALSE;
            }
        }
    }

    return allowTearing == TRUE;
}

void Sample::loadPipeline()
{
	device = std::make_unique<Device>(useWarp);
	commandQueue = std::make_unique<CommandQueue>(*device);
	swapChain = std::make_unique<SwapChain>(handleWin, *commandQueue, width, heigth, allowTearing);
    RTVdescriptorHeap = std::make_unique<DescriptorHeap>(*device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, SwapChain::FrameCount);

    swapChain->UpdateRenderTargetViews(*device, *RTVdescriptorHeap);

    for (int i = 0; i < SwapChain::FrameCount; ++i)
        commandAllocators[i] = std::make_unique<CommandAllocator>(*device, D3D12_COMMAND_LIST_TYPE_DIRECT);

    commandList = std::make_unique<CommandList>(*device, *commandAllocators[swapChain->GetCurrentBackBufferIndex()], D3D12_COMMAND_LIST_TYPE_DIRECT);
    fence = std::make_unique<Fence>(*device);
}

void Sample::parseCommandLineArguments()
{
    int argc;
    wchar_t** argv = ::CommandLineToArgvW(::GetCommandLineW(), &argc);
    for (int i = 1; i < argc; ++i)
    {
        if (_wcsnicmp(argv[i], L"-warp", wcslen(argv[i])) == 0 ||
            _wcsnicmp(argv[i], L"/warp", wcslen(argv[i])) == 0)
        {
            useWarp = true;
        }
    }
    ::LocalFree(argv);
}

void Sample::enableDebugLayer()
{
#if defined(_DEBUG)
    // always enable the debug layer before doing anything DX12 related
    // so all possible errors generated while creating DX12 objects
    // are caught by the debug layer.
    ComPtr<ID3D12Debug> debugInterface;
    ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)));
    debugInterface->EnableDebugLayer();
#endif
}

#pragma endregion