#include "PrecompiledHeaders.h"
#include "pipeline/CommandList.h"

#include "pipeline/CommandAllocator.h"
#include "pipeline/DescriptorHeap.h"
#include "pipeline/Device.h"
#include "resources/Resource.h"

#pragma region Public Methods

CommandList::CommandList(const Device& device, const CommandAllocator& commandAllocator, D3D12_COMMAND_LIST_TYPE commandListType)
{
    ThrowIfFailed(device.Get()->CreateCommandList(0, commandListType, commandAllocator.GetPtr(), nullptr, IID_PPV_ARGS(&commandList)));
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

void CommandList::Populate(const DescriptorHeap& RTVdescriptorHeap, const DescriptorHeap& DSVdescriptorHeap, CommandAllocator& commandAllocator, const Resource& backBuffer, UINT currentBackBufferIndex)
{
    commandAllocator.Reset();
    commandList->Reset(commandAllocator.GetPtr(), nullptr);

    // clear the render target.
    TransitionResource(backBuffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

    FLOAT clearColor[] = { 0.78f, 0.549f, 0.941f, 1.0f };
    ClearRTV(RTVdescriptorHeap.GetRenderTargetView(currentBackBufferIndex), clearColor);
    ClearDepth(DSVdescriptorHeap.GetCPUDescriptorHandleForHeapStart());

    // present
    TransitionResource(backBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    ThrowIfFailed(commandList->Close());
}

void CommandList::TransitionResource(const Resource& resource, D3D12_RESOURCE_STATES beforeState, D3D12_RESOURCE_STATES afterState)
{
    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(resource.GetPtr(), beforeState, afterState);
    commandList->ResourceBarrier(1, &barrier);
}

void CommandList::ClearRTV(D3D12_CPU_DESCRIPTOR_HANDLE rtv, FLOAT* clearColor)
{
    commandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
}

void CommandList::ClearDepth(D3D12_CPU_DESCRIPTOR_HANDLE dsv, FLOAT depth)
{
    commandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH, depth, 0, 0, nullptr);
}

#pragma endregion

#pragma region Private Methods

#pragma endregion