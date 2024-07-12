#pragma once

#include <memory>

#include "pipeline/CommandAllocator.h"
#include "pipeline/CommandList.h"
#include "pipeline/CommandQueue.h"
#include "pipeline/DescriptorHeap.h"
#include "pipeline/Device.h"
#include "pipeline/Fence.h"
#include "pipeline/SwapChain.h"

class Sample
{
public:
	Sample(HWND hWnd, uint32_t width, uint32_t heigth, bool useWarp);

	void OnInit();
	void OnUpdate();
	void OnRender();
	void OnDestroy();

private:
	bool checkTearingSupport();
	void loadPipeline();

private:
	HWND handleWin;

	uint32_t width;
	uint32_t heigth;

	bool useWarp;
	bool allowTearing;
	bool VSync = true;

	uint64_t fenceValue = 0;
	uint64_t frameFenceValues[SwapChain::FrameCount] = {};

	std::unique_ptr<Device> device;
	std::unique_ptr<CommandQueue> commandQueue;
	std::unique_ptr<SwapChain> swapChain;
	std::unique_ptr<DescriptorHeap> RTVdescriptorHeap;
	std::unique_ptr<CommandAllocator> commandAllocators[SwapChain::FrameCount];
	std::unique_ptr<CommandList> commandList;
	std::unique_ptr<Fence> fence;
};