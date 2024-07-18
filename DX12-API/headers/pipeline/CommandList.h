#pragma once

#include "helpers/Helpers.h"

class CommandAllocator;
class DescriptorHeap;
class Device;
class Resource;

class CommandList
{
public:
	CommandList(const Device& device, const CommandAllocator& commandAllocator, D3D12_COMMAND_LIST_TYPE commandListType);

	const ComPtr<ID3D12GraphicsCommandList>& Get();
	ID3D12GraphicsCommandList* GetPtr();

	void Populate(const DescriptorHeap& RTVdescriptorHeap, const DescriptorHeap& DSVdescriptorHeap, CommandAllocator& commandAllocator, const Resource& backBuffer, UINT currentBackBufferIndex);

	void TransitionResource(const Resource& resource, D3D12_RESOURCE_STATES beforeState, D3D12_RESOURCE_STATES afterState);
	void ClearRTV(D3D12_CPU_DESCRIPTOR_HANDLE rtv, FLOAT* clearColor);
	void ClearDepth(D3D12_CPU_DESCRIPTOR_HANDLE dsv, FLOAT depth = 1.0f);
private:

private:
	ComPtr<ID3D12GraphicsCommandList> commandList;
};