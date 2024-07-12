#pragma once

#include "pipeline/Device.h"

class CommandAllocator;

class CommandList
{
public:
	CommandList(const Device& device, const CommandAllocator& commandAllocator, D3D12_COMMAND_LIST_TYPE commandListType);

	const ComPtr<ID3D12GraphicsCommandList>& Get();

	HRESULT Reset(const CommandAllocator& commandAllocator, ID3D12PipelineState* pInitialState);
	void ResourceBarrier(UINT numBarriers, const D3D12_RESOURCE_BARRIER* pBarriers);
	void ClearRenderTargetView(D3D12_CPU_DESCRIPTOR_HANDLE RenderTargetView, const FLOAT ColorRGBA[4], UINT NumRects, const D3D12_RECT* pRects);
	HRESULT Close();

private:
	ComPtr<ID3D12GraphicsCommandList> commandList;
};