#include "PrecompiledHeaders.h"
#include "pipeline/CommandQueue.h"

#pragma region Public Methods

CommandQueue::CommandQueue(const Device& device)
{
	D3D12_COMMAND_QUEUE_DESC queueDescriptor = {};
	queueDescriptor.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDescriptor.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	ThrowIfFailed(device.Get()->CreateCommandQueue(&queueDescriptor, IID_PPV_ARGS(&commandQueue)));
}

const ComPtr<ID3D12CommandQueue>& CommandQueue::Get() const
{
	return commandQueue;
}

ID3D12CommandQueue* CommandQueue::GetPtr() const
{
	return commandQueue.Get();
}

#pragma endregion