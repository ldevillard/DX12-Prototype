#include "PrecompiledHeaders.h"
#include "pipeline/Fence.h"

#include "pipeline/CommandQueue.h"
#include "pipeline/Device.h"

#pragma region Public Methods

Fence::Fence(const Device& device)
{
	ThrowIfFailed(device.Get()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
	createFenceEventHandle();
}

#pragma region Getters

HANDLE Fence::GetEvent() const
{
	return fenceEvent;
}

#pragma endregion

uint64_t Fence::Signal(const CommandQueue& commandQueue)
{
	uint64_t fenceValueForSignal = ++fenceValue;
	ThrowIfFailed(commandQueue.Get()->Signal(fence.Get(), fenceValueForSignal));

	return fenceValueForSignal;
}

void Fence::WaitForFenceValue(uint64_t fValue, std::chrono::milliseconds duration)
{
	if (fence->GetCompletedValue() < fValue)
	{
		ThrowIfFailed(fence->SetEventOnCompletion(fValue, fenceEvent));
		::WaitForSingleObject(fenceEvent, static_cast<DWORD>(duration.count()));
	}
}

void Fence::Flush(const CommandQueue& commandQueue)
{
	uint64_t fenceValueForSignal = Signal(commandQueue);
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