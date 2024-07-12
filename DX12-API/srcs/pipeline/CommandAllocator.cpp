#include "PrecompiledHeaders.h"
#include "pipeline/CommandAllocator.h"

#pragma region Public Methods

CommandAllocator::CommandAllocator(const Device& device, D3D12_COMMAND_LIST_TYPE commandListType)
{
	ThrowIfFailed(device.Get()->CreateCommandAllocator(commandListType, IID_PPV_ARGS(&commandAllocator)));
}

const ComPtr<ID3D12CommandAllocator>& CommandAllocator::Get() const
{
	return commandAllocator;
}

HRESULT CommandAllocator::Reset()
{
	return commandAllocator->Reset();
}

#pragma endregion