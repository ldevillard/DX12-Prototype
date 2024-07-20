#pragma once

#include "helpers/Helpers.h"

class Device;

class CommandAllocator
{
public:
	CommandAllocator(const Device& device, D3D12_COMMAND_LIST_TYPE commandListType);

	// getters
	const ComPtr<ID3D12CommandAllocator>& Get() const;
	ID3D12CommandAllocator* GetPtr() const;

	HRESULT Reset();

private:
	ComPtr<ID3D12CommandAllocator> commandAllocator;
};