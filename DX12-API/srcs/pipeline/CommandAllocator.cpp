#include "PrecompiledHeaders.h"
#include "pipeline/CommandAllocator.h"

#include "pipeline/Device.h"

#pragma region Public Methods

CommandAllocator::CommandAllocator(const Device& device, D3D12_COMMAND_LIST_TYPE commandListType)
{
	ThrowIfFailed(device.Get()->CreateCommandAllocator(commandListType, IID_PPV_ARGS(&commandAllocator)));
}

#pragma region Getters

const ComPtr<ID3D12CommandAllocator>& CommandAllocator::Get() const
{
	return commandAllocator;
}

ID3D12CommandAllocator* CommandAllocator::GetPtr() const
{
	return commandAllocator.Get();
}

#pragma endregion

HRESULT CommandAllocator::Reset()
{
	return commandAllocator->Reset();
}

#pragma endregion