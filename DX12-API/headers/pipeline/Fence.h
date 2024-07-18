#pragma once

#include <chrono>

#include "helpers/Helpers.h"

class CommandQueue;
class Device;

class Fence
{
public:
	Fence(const Device& device);

	HANDLE GetEvent() const;

    uint64_t Signal(const CommandQueue& commandQueue);
    void WaitForFenceValue(uint64_t fValue, std::chrono::milliseconds duration = std::chrono::milliseconds::max());
	void Flush(const CommandQueue& commandQueue);

private:
	void createFenceEventHandle();

private:
	HANDLE fenceEvent;
	uint64_t fenceValue = 0;

	ComPtr<ID3D12Fence> fence;
};
