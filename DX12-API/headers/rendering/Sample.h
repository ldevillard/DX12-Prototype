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
	void loadAssets();
	void parseCommandLineArguments();
	void enableDebugLayer();
	// create a GPU buffer.
	void updateBufferResource(Resource& destinationResource, Resource& intermediateResource,
						      size_t numElements, size_t elementSize, const void* bufferData,
						      D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE);
	void resizeDepthBuffer(int width, int height);

private:
	HWND handleWin;

	uint32_t width;
	uint32_t height;

	bool useWarp;
	bool allowTearing;
	bool vSync = true;
	float fov = 60.0f;

	Timer timer;

	uint64_t frameFenceValues[SwapChain::FrameCount] = {};
	std::unique_ptr<Device> device;
	std::unique_ptr<CommandQueue> commandQueue;
	std::unique_ptr<SwapChain> swapChain;
	std::unique_ptr<DescriptorHeap> RTVdescriptorHeap;
	std::unique_ptr<CommandAllocator> commandAllocators[SwapChain::FrameCount];
	std::unique_ptr<CommandList> commandList;
	std::unique_ptr<Fence> fence;

	// vertex buffer for the cube.
	Resource vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	// index buffer for the cube.
	Resource indexBuffer;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;
	// depth buffer.
	Resource depthBuffer;
	// descriptor heap for depth buffer.
	std::unique_ptr<DescriptorHeap> DSVdescriptorHeap;

	// root signature
	ComPtr<ID3D12RootSignature> rootSignature;
	// pipeline state object.
	ComPtr<ID3D12PipelineState> pipelineState;

	D3D12_VIEWPORT viewport;
	D3D12_RECT scissorRect;

	DirectX::XMMATRIX modelMatrix;
	DirectX::XMMATRIX viewMatrix;
	DirectX::XMMATRIX projectionMatrix;
};