#pragma once

#include "pipeline/Device.h"
#include "resources/Resource.h"

class CommandQueue;
class DescriptorHeap;

class SwapChain
{
public:
	SwapChain(HWND hWnd, const CommandQueue& commandQueue, uint32_t width, uint32_t height, bool allowTearing);

	const ComPtr<IDXGISwapChain4>& Get() const;
	const UINT GetCurrentBackBufferIndex();
	const Resource& GetCurrentBackBuffer() const;

	void UpdateRenderTargetViews(const Device& device, const DescriptorHeap& descriptorHeap);
	void ResetBackBuffer(int index);
	void Resize(uint32_t width, uint32_t height);

	static constexpr UINT FrameCount = 3;

private:
	UINT currentBackBufferIndex;
	ComPtr<IDXGISwapChain4> swapChain;
	Resource backBuffers[FrameCount];
};