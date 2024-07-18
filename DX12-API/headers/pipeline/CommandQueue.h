#pragma once

#include "helpers/Helpers.h"

class Device;

class CommandQueue
{
public:
	CommandQueue(const Device& device);

	const ComPtr<ID3D12CommandQueue>& Get() const;
	ID3D12CommandQueue* GetPtr() const;

private:
	ComPtr<ID3D12CommandQueue> commandQueue;
};