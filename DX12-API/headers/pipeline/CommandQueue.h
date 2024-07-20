#pragma once

#include "helpers/Helpers.h"

class Device;

class CommandQueue
{
public:
	CommandQueue(const Device& device);
	
	// getters
	const ComPtr<ID3D12CommandQueue>& Get() const;
	ID3D12CommandQueue* GetPtr() const;

	void ExecuteCommandLists(UINT commandListsCount, ID3D12CommandList* const* commandLists);

private:
	ComPtr<ID3D12CommandQueue> commandQueue;
};