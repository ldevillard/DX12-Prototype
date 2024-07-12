#include "PrecompiledHeaders.h"
#include "pipeline/CommandList.h"

#include "pipeline/CommandAllocator.h"

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

HRESULT CommandList::Reset(const CommandAllocator& commandAllocator, ID3D12PipelineState* pInitialState)
{
    return commandList->Reset(commandAllocator.Get().Get(), pInitialState);
}

void CommandList::ResourceBarrier(UINT numBarriers, const D3D12_RESOURCE_BARRIER* pBarriers)
{
    commandList->ResourceBarrier(numBarriers, pBarriers);
}

void CommandList::ClearRenderTargetView(D3D12_CPU_DESCRIPTOR_HANDLE RenderTargetView, const FLOAT ColorRGBA[4], UINT NumRects, const D3D12_RECT* pRects)
{
    commandList->ClearRenderTargetView(RenderTargetView, ColorRGBA, NumRects, pRects);
}

HRESULT CommandList::Close()
{
    return commandList->Close();
}

#pragma endregion