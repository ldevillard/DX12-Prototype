#pragma once

#include <memory>

#include "objects/Camera.h"
#include "pipeline/CommandAllocator.h"
#include "pipeline/CommandList.h"
#include "pipeline/CommandQueue.h"
#include "pipeline/DescriptorHeap.h"
#include "pipeline/Device.h"
#include "pipeline/Fence.h"
#include "pipeline/PipelineStateObject.h"
#include "pipeline/SwapChain.h"
#include "resources/VertexBuffer.h"

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

	void ProcessCameraInputs(float x, float y, float z, bool accelerate);
	void ProcessCameraMouseMovement(float xOffset, float yOffset);
	void ProcessCameraMouseScroll(float offset);

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

	void initImGui(HWND hWnd);

private:
	HWND handleWin;

	uint32_t width;
	uint32_t height;

	bool useWarp;
	bool allowTearing;
	bool vSync = true;

	Camera camera;

	uint64_t frameFenceValues[SwapChain::FrameCount] = {};
	std::unique_ptr<Device> device;
	std::unique_ptr<CommandQueue> commandQueue;
	std::unique_ptr<SwapChain> swapChain;
	std::unique_ptr<DescriptorHeap> RTVdescriptorHeap;
	std::unique_ptr<DescriptorHeap> SRVdescriptorHeap;
	std::unique_ptr<CommandAllocator> commandAllocators[SwapChain::FrameCount];
	std::unique_ptr<CommandList> commandList;
	std::unique_ptr<Fence> fence;
	std::unique_ptr<PipelineStateObject> pipelineStateObject;

	VertexBuffer vertexBuffer;

	// index buffer for the cube.
	Resource indexBuffer;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;
	// depth buffer.
	Resource depthBuffer;
	// descriptor heap for depth buffer.
	std::unique_ptr<DescriptorHeap> DSVdescriptorHeap;

	// root signature
	ComPtr<ID3D12RootSignature> rootSignature;

	D3D12_VIEWPORT viewport;
	D3D12_RECT scissorRect;

	DirectX::XMMATRIX modelMatrix;
};