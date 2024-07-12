#pragma once

#include "pipeline/Device.h"

class CommandQueue;
class DescriptorHeap;

class SwapChain
{
public:
	SwapChain(HWND hWnd, const CommandQueue& commandQueue, uint32_t width, uint32_t height, bool allowTearing);

	const ComPtr<IDXGISwapChain4>& Get() const;
	const UINT GetCurrentBackBufferIndex();
	const ComPtr<ID3D12Resource>& GetCurrentBackBuffer() const;

	void UpdateRenderTargetViews(const Device& device, const DescriptorHeap& descriptorHeap);

	static constexpr UINT FrameCount = 3;

private:
	UINT currentBackBufferIndex;
	ComPtr<IDXGISwapChain4> swapChain;
	ComPtr<ID3D12Resource> backBuffers[FrameCount];
};