#pragma once

#include "pipeline/Device.h"

class CommandAllocator;
class DescriptorHeap;
class Resource;

class CommandList
{
public:
	CommandList(const Device& device, const CommandAllocator& commandAllocator, D3D12_COMMAND_LIST_TYPE commandListType);

	const ComPtr<ID3D12GraphicsCommandList>& Get();
	ID3D12GraphicsCommandList* GetPtr();

	void Populate(const DescriptorHeap& RTVdescriptorHeap, CommandAllocator& commandAllocator, const Resource& backBuffer, UINT currentBackBufferIndex);
	
private:
	ComPtr<ID3D12GraphicsCommandList> commandList;
};