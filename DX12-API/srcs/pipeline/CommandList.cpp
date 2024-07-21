#include "PrecompiledHeaders.h"
#include "pipeline/CommandList.h"

#include "pipeline/CommandAllocator.h"
#include "pipeline/DescriptorHeap.h"
#include "pipeline/Device.h"
#include "pipeline/PipelineStateObject.h"
#include "pipeline/RootSignature.h"
#include "resources/IndexBuffer.h"
#include "resources/Resource.h"
#include "resources/VertexBuffer.h"

#pragma region Public Methods

CommandList::CommandList(const Device& device, const CommandAllocator& commandAllocator, D3D12_COMMAND_LIST_TYPE commandListType)
{
    ThrowIfFailed(device.Get()->CreateCommandList(0, commandListType, commandAllocator.GetPtr(), nullptr, IID_PPV_ARGS(&commandList)));
}

#pragma region Getters

const ComPtr<ID3D12GraphicsCommandList>& CommandList::Get()
{
    return commandList;
}

ID3D12GraphicsCommandList* CommandList::GetPtr()
{
    return commandList.Get();
}

#pragma endregion

#pragma region Setters

void CommandList::SetPipelineState(const PipelineStateObject& pso)
{
    commandList->SetPipelineState(pso.GetPtr());
}

void CommandList::SetGraphicsRootSignature(const RootSignature& rootSignature)
{
    commandList->SetGraphicsRootSignature(rootSignature.GetPtr());
}

#pragma endregion

void CommandList::ClearRenderTargets(const Resource& backBuffer, D3D12_CPU_DESCRIPTOR_HANDLE rtv, D3D12_CPU_DESCRIPTOR_HANDLE dsv, Vector4 clearColor)
{
    TransitionResource(backBuffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    
    FLOAT outArray[4];
    Vec4ToFloat(clearColor, outArray);

    ClearRTV(rtv, outArray);
    ClearDepth(dsv);
}

void CommandList::ClearDepth(D3D12_CPU_DESCRIPTOR_HANDLE dsv, FLOAT depth)
{
    commandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH, depth, 0, 0, nullptr);
}

void CommandList::ClearRTV(D3D12_CPU_DESCRIPTOR_HANDLE rtv, FLOAT* clearColor)
{
    commandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
}

void CommandList::Close()
{
    ThrowIfFailed(commandList->Close());
}

void CommandList::PrepareInputAssemblerStage(const VertexBuffer& vertexBuffer, const IndexBuffer& indexBuffer, D3D12_PRIMITIVE_TOPOLOGY primitiveTopology)
{
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView = vertexBuffer.GetVertexBufferView();
    D3D12_INDEX_BUFFER_VIEW indexBufferView = indexBuffer.GetIndexBufferView();
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
    commandList->IASetIndexBuffer(&indexBufferView);
}

void CommandList::PrepareOutputMergerStage(D3D12_CPU_DESCRIPTOR_HANDLE* rtv, D3D12_CPU_DESCRIPTOR_HANDLE* dsv)
{
    commandList->OMSetRenderTargets(1, rtv, FALSE, dsv);
}

void CommandList::PrepareRasterizerStage(const D3D12_VIEWPORT& viewport, const D3D12_RECT& scissorRect)
{
    commandList->RSSetViewports(1, &viewport);
    commandList->RSSetScissorRects(1, &scissorRect);
}

void CommandList::Reset(const CommandAllocator& commandAllocator, const PipelineStateObject initialState)
{
    ThrowIfFailed(commandList->Reset(commandAllocator.GetPtr(), initialState.GetPtr()));
}

void CommandList::TransitionResource(const Resource& resource, D3D12_RESOURCE_STATES beforeState, D3D12_RESOURCE_STATES afterState)
{
    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(resource.GetPtr(), beforeState, afterState);
    commandList->ResourceBarrier(1, &barrier);
}

#pragma endregion