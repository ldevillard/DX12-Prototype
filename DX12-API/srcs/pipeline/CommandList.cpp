#include "PrecompiledHeaders.h"
#include "pipeline/CommandList.h"

#include "pipeline/CommandAllocator.h"
#include "pipeline/DescriptorHeap.h"
#include "resources/Resource.h"

#pragma region Public Methods

CommandList::CommandList(const Device& device, const CommandAllocator& commandAllocator, D3D12_COMMAND_LIST_TYPE commandListType)
{
    ThrowIfFailed(device.Get()->CreateCommandList(0, commandListType, commandAllocator.Get().Get(), nullptr, IID_PPV_ARGS(&commandList)));

    ThrowIfFailed(commandList->Close());
}

const ComPtr<ID3D12GraphicsCommandList>& CommandList::Get()
{
    return commandList;
}

ID3D12GraphicsCommandList* CommandList::GetPtr()
{
    return commandList.Get();
}

void CommandList::Populate(const DescriptorHeap& RTVdescriptorHeap, CommandAllocator& commandAllocator, const Resource& backBuffer, UINT currentBackBufferIndex)
{
    commandAllocator.Reset();
    commandList->Reset(commandAllocator.GetPtr(), nullptr);

    // clear the render target.
    {
        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            backBuffer.GetPtr(),
            D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

        commandList->ResourceBarrier(1, &barrier);

        FLOAT clearColor[] = { 0.4f, 0.6f, 0.9f, 1.0f };
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(RTVdescriptorHeap.Get()->GetCPUDescriptorHandleForHeapStart(),
            currentBackBufferIndex, RTVdescriptorHeap.GetRTVDescriptorSize());

        commandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
    }

    // present
    {
        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            backBuffer.GetPtr(),
            D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
        commandList->ResourceBarrier(1, &barrier);

        ThrowIfFailed(commandList->Close());
    }
}

#pragma endregion