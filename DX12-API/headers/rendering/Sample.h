#pragma once

#include <memory>

#include "helpers/Timer.h"
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
	Sample(uint32_t width, uint32_t height);

	void OnInit(HWND hWnd);
	void OnUpdate();
	void OnRender();
	void OnDestroy();

	void ToggleVSync();
	void Resize(uint32_t width, uint32_t height);

private:
	bool checkTearingSupport();
	void loadPipeline();
	void parseCommandLineArguments();
	void enableDebugLayer();

private:
	HWND handleWin;

	uint32_t width;
	uint32_t height;

	bool useWarp;
	bool allowTearing;
	bool vSync = true;

	Timer timer;

	uint64_t frameFenceValues[SwapChain::FrameCount] = {};

	std::unique_ptr<Device> device;
	std::unique_ptr<CommandQueue> commandQueue;
	std::unique_ptr<SwapChain> swapChain;
	std::unique_ptr<DescriptorHeap> RTVdescriptorHeap;
	std::unique_ptr<CommandAllocator> commandAllocators[SwapChain::FrameCount];
	std::unique_ptr<CommandList> commandList;
	std::unique_ptr<Fence> fence;
};