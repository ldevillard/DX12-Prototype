#pragma once

#include <memory>

#include "pipeline/CommandAllocator.h"
#include "pipeline/CommandList.h"
#include "pipeline/CommandQueue.h"
#include "pipeline/DescriptorHeap.h"
#include "pipeline/Device.h"
#include "pipeline/Fence.h"
#include "pipeline/PipelineStateObject.h"
#include "pipeline/RootSignature.h"
#include "pipeline/SwapChain.h"
#include "resources/IndexBuffer.h"
#include "resources/VertexBuffer.h"

class Sample
{
public:
	Sample(uint32_t width, uint32_t height);

	CommandList& GetCommandList();
	RootSignature& GetRootSignature();

	void OnInit(HWND hWnd);
	void OnUpdate();
	void OnRender();
	void OnDestroy();

	void Resize(uint32_t width, uint32_t height);
	void SetupPipeline();
	void ToggleVSync();

	// create a GPU buffer.
	void UpdateBufferResource(Resource& destinationResource, Resource& intermediateResource,
		size_t elementsCount, size_t elementSize, const void* bufferData,
		D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE) const;

private:
	bool checkTearingSupport();
	void enableDebugLayer();
	void initPipeline();
	void initImGui(HWND hWnd);
	void parseCommandLineArguments();
	void preRender();
	void resizeDepthBuffer(int width, int height);

private:
	static constexpr Vector clearColor = { 0.4f, 0.6f, 0.9f, 1.0f };

	HWND hWnd;

	uint32_t width;
	uint32_t height;

	bool useWarp;
	bool allowTearing;
	bool vSync = true;

	uint64_t frameFenceValues[SwapChain::FrameCount] = {};

	// pipeline objects
	std::unique_ptr<CommandAllocator> commandAllocators[SwapChain::FrameCount];
	std::unique_ptr<CommandList> commandList;
	std::unique_ptr<CommandQueue> commandQueue;
	std::unique_ptr<Device> device;
	std::unique_ptr<Fence> fence;
	std::unique_ptr<PipelineStateObject> pipelineStateObject;
	std::unique_ptr<RootSignature> rootSignature;
	std::unique_ptr<DescriptorHeap> RTVdescriptorHeap;
	std::unique_ptr<DescriptorHeap> SRVdescriptorHeap;
	std::unique_ptr<SwapChain> swapChain;

	// depth buffer and its descriptor heap
	Buffer depthBuffer;
	std::unique_ptr<DescriptorHeap> DSVdescriptorHeap;

	D3D12_VIEWPORT viewport;
	D3D12_RECT scissorRect;
};