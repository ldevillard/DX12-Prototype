#pragma once

#include <chrono>

#include "pipeline/Device.h"

class CommandQueue;

class Fence
{
public:
	Fence(const Device& device);

	HANDLE GetEvent() const;

    uint64_t Signal(const CommandQueue& commandQueue, uint64_t& fenceValue);
    void WaitForFenceValue(uint64_t fenceValue, std::chrono::milliseconds duration = std::chrono::milliseconds::max());
	void Flush(const CommandQueue& commandQueue, uint64_t& fenceValue);

private:
	void createFenceEventHandle();

private:
	HANDLE fenceEvent;
	ComPtr<ID3D12Fence> fence;
};
