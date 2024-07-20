#pragma once

#include "helpers/Helpers.h"
#include "maths/Maths.h"

class CommandAllocator;
class DescriptorHeap;
class Device;
class IndexBuffer;
class PipelineStateObject;
class Resource;
class VertexBuffer;

class CommandList
{
public:
	CommandList(const Device& device, const CommandAllocator& commandAllocator, D3D12_COMMAND_LIST_TYPE commandListType);

	const ComPtr<ID3D12GraphicsCommandList>& Get();
	ID3D12GraphicsCommandList* GetPtr();

	void ClearRenderTargets(const Resource& backBuffer, D3D12_CPU_DESCRIPTOR_HANDLE rtv, D3D12_CPU_DESCRIPTOR_HANDLE dsv, Vector4 clearColor = { 1, 1, 1, 1 });
	void ClearDepth(D3D12_CPU_DESCRIPTOR_HANDLE dsv, FLOAT depth = 1.0f);
	void ClearRTV(D3D12_CPU_DESCRIPTOR_HANDLE rtv, FLOAT* clearColor);
	
	void PrepareInputAssemblerStage(const VertexBuffer& vertexBuffer, const IndexBuffer& indexBuffer, D3D12_PRIMITIVE_TOPOLOGY primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	void PrepareOutputMergerStage(D3D12_CPU_DESCRIPTOR_HANDLE* rtv, D3D12_CPU_DESCRIPTOR_HANDLE* dsv);
	void PrepareRasterizerStage(const D3D12_VIEWPORT& viewport, const D3D12_RECT& scissorRect);
	
	void Reset(const CommandAllocator& commandAllocator, const PipelineStateObject initialState);
	void TransitionResource(const Resource& resource, D3D12_RESOURCE_STATES beforeState, D3D12_RESOURCE_STATES afterState);

private:
	ComPtr<ID3D12GraphicsCommandList> commandList;
};