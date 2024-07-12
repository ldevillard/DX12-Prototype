#include "PrecompiledHeaders.h"
#include "pipeline/Fence.h"

#include "pipeline/CommandQueue.h"

#pragma region Public Methods

Fence::Fence(const Device& device)
{
	ThrowIfFailed(device.Get()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
	createFenceEventHandle();
}

HANDLE Fence::GetEvent() const
{
	return fenceEvent;
}

uint64_t Fence::Signal(const CommandQueue& commandQueue, uint64_t &fenceValue)
{
	uint64_t fenceValueForSignal = ++fenceValue;
	ThrowIfFailed(commandQueue.Get()->Signal(fence.Get(), fenceValueForSignal));

	return fenceValueForSignal;
}

void Fence::WaitForFenceValue(uint64_t fenceValue, std::chrono::milliseconds duration)
{
	if (fence->GetCompletedValue() < fenceValue)
	{
		ThrowIfFailed(fence->SetEventOnCompletion(fenceValue, fenceEvent));
		::WaitForSingleObject(fenceEvent, static_cast<DWORD>(duration.count()));
	}
}

void Fence::Flush(const CommandQueue& commandQueue, uint64_t& fenceValue)
{
	uint64_t fenceValueForSignal = Signal(commandQueue, fenceValue);
	WaitForFenceValue(fenceValueForSignal);
}

#pragma endregion

#pragma region Private Methods

void Fence::createFenceEventHandle()
{
	fenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent && "Failed to create fence event.");
}

#pragma endregion