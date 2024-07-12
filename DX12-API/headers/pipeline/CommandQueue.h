#pragma once

#include "pipeline/Device.h"

class CommandQueue
{
public:
	CommandQueue(const Device& device);

	const ComPtr<ID3D12CommandQueue>& Get() const;

private:
	ComPtr<ID3D12CommandQueue> commandQueue;
};