#include "PrecompiledHeaders.h"
#include "rendering/Sample.h"

#include <chrono>

#pragma region Public Methods

Sample::Sample(HWND hWnd, uint32_t w, uint32_t h, bool warp)
	: handleWin(hWnd)
	, width(w)
	, heigth(h)
	, useWarp(warp)
{
	allowTearing = checkTearingSupport();
}

void Sample::OnInit()
{
	loadPipeline();
}

void Sample::OnUpdate()
{
    static uint64_t frameCounter = 0;
    static double elapsedSeconds = 0.0;
    static std::chrono::high_resolution_clock clock;
    static auto t0 = clock.now();

    frameCounter++;
    auto t1 = clock.now();
    auto deltaTime = t1 - t0;
    t0 = t1;

    elapsedSeconds += deltaTime.count() * 1e-9;
    if (elapsedSeconds > 1.0)
    {
        char buffer[500];
        auto fps = frameCounter / elapsedSeconds;
        sprintf_s(buffer, 500, "FPS: %f\n", fps);
        OutputDebugStringA(buffer);

        frameCounter = 0;
        elapsedSeconds = 0.0;
    }
}

void Sample::OnRender()
{
    UINT currentBackBufferIndex = swapChain->GetCurrentBackBufferIndex();
    std::unique_ptr<CommandAllocator>& commandAllocator = commandAllocators[currentBackBufferIndex];
    ComPtr<ID3D12Resource> backBuffer = swapChain->GetCurrentBackBuffer();

    commandAllocator->Reset();
    commandList->Reset(*commandAllocator, nullptr);

    // Clear the render target.
    {
        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            backBuffer.Get(),
            D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

        commandList->ResourceBarrier(1, &barrier);

        FLOAT clearColor[] = { 0.4f, 0.6f, 0.9f, 1.0f };
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(RTVdescriptorHeap->Get()->GetCPUDescriptorHandleForHeapStart(),
            currentBackBufferIndex, RTVdescriptorHeap->GetRTVDescriptorSize());

        commandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
    }

    // Present
    {
        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            backBuffer.Get(),
            D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
        commandList->ResourceBarrier(1, &barrier);

        ThrowIfFailed(commandList->Close());

        ID3D12CommandList* const commandLists[] = {
            commandList->Get().Get()
        };
        commandQueue->Get()->ExecuteCommandLists(_countof(commandLists), commandLists);

        frameFenceValues[currentBackBufferIndex] = fence->Signal(*commandQueue, fenceValue);

        UINT syncInterval = VSync ? 1 : 0;
        UINT presentFlags = allowTearing && !VSync ? DXGI_PRESENT_ALLOW_TEARING : 0;
        ThrowIfFailed(swapChain->Get()->Present(syncInterval, presentFlags));

        currentBackBufferIndex = swapChain->GetCurrentBackBufferIndex();

        fence->WaitForFenceValue(frameFenceValues[currentBackBufferIndex]);
    }
}

void Sample::OnDestroy()
{
    fence->Flush(*commandQueue, fenceValue);
    CloseHandle(fence->GetEvent());
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

#pragma endregion