#pragma once

#include "pipeline/Device.h"

class CommandAllocator
{
public:
	CommandAllocator(const Device& device, D3D12_COMMAND_LIST_TYPE commandListType);

	const ComPtr<ID3D12CommandAllocator>& Get() const;
	ID3D12CommandAllocator* GetPtr() const;

	HRESULT Reset();

private:
	ComPtr<ID3D12CommandAllocator> commandAllocator;
};