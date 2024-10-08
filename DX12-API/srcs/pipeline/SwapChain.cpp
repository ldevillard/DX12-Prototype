#include "PrecompiledHeaders.h"
#include "pipeline/SwapChain.h"

#include "pipeline/CommandQueue.h"
#include "pipeline/DescriptorHeap.h"
#include "pipeline/Device.h"

#pragma region Public Methods

SwapChain::SwapChain(HWND hWnd, const CommandQueue& commandQueue, uint32_t width, uint32_t height, bool allowTearing)
{
    ComPtr<IDXGIFactory4> dxgiFactory4;
    UINT createFactoryFlags = 0;
#if defined(_DEBUG)
    createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

    ThrowIfFailed(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory4)));

    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = FrameCount;
    swapChainDesc.Width = width;
    swapChainDesc.Height = height;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1;
    // it is recommended to always allow tearing if tearing support is available.
    swapChainDesc.Flags = allowTearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

    ComPtr<IDXGISwapChain1> swapChain1;
    ThrowIfFailed(dxgiFactory4->CreateSwapChainForHwnd(
        commandQueue.GetPtr(),
        hWnd,
        &swapChainDesc,
        nullptr,
        nullptr,
        &swapChain1));

    // disable the Alt+Enter fullscreen toggle feature. Switching to fullscreen
    // will be handled manually.
    ThrowIfFailed(dxgiFactory4->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER));

    ThrowIfFailed(swapChain1.As(&swapChain));

    currentBackBufferIndex = swapChain->GetCurrentBackBufferIndex();
}

#pragma region Getters

const ComPtr<IDXGISwapChain4>& SwapChain::Get() const
{
    return swapChain;
}

const UINT SwapChain::GetCurrentBackBufferIndex()
{
    currentBackBufferIndex = swapChain->GetCurrentBackBufferIndex();
    return currentBackBufferIndex;
}

const Resource& SwapChain::GetCurrentBackBuffer() const
{
    return backBuffers[currentBackBufferIndex];
}

#pragma endregion

void SwapChain::Present(bool vSync, bool allowTearing)
{
    UINT syncInterval = vSync ? 1 : 0;
    UINT flags = allowTearing && !vSync ? DXGI_PRESENT_ALLOW_TEARING : 0;
    ThrowIfFailed(swapChain->Present(syncInterval, flags));
}

void SwapChain::ResetBackBuffer(int index)
{
    backBuffers[index].Reset();
}

void SwapChain::Resize(uint32_t width, uint32_t height)
{
    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    ThrowIfFailed(swapChain->GetDesc(&swapChainDesc));
    ThrowIfFailed(swapChain->ResizeBuffers(FrameCount, width, height,
        swapChainDesc.BufferDesc.Format, swapChainDesc.Flags));

    currentBackBufferIndex = swapChain->GetCurrentBackBufferIndex();
}

void SwapChain::UpdateRenderTargetViews(const Device& device, const DescriptorHeap& descriptorHeap)
{
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(descriptorHeap.Get()->GetCPUDescriptorHandleForHeapStart());
    UINT rtvDescriptorSize = descriptorHeap.GetDescriptorSize();

    for (int i = 0; i < FrameCount; ++i)
    {
        ComPtr<ID3D12Resource> backBuffer;
        ThrowIfFailed(swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer)));

        device.Get()->CreateRenderTargetView(backBuffer.Get(), nullptr, rtvHandle);

        backBuffers[i] = Resource(backBuffer);

        rtvHandle.Offset(rtvDescriptorSize);
    }
}

#pragma endregion