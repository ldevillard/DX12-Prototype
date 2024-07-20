#pragma once

#include "helpers/Helpers.h"

class Device;

class DescriptorHeap
{
public:
	DescriptorHeap(const Device& device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors, D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

	// getters
	const ComPtr<ID3D12DescriptorHeap>& Get() const;
	ID3D12DescriptorHeap* GetPtr() const;
	const UINT GetDescriptorSize() const;
	const CD3DX12_CPU_DESCRIPTOR_HANDLE GetRenderTargetView(int currentBackBufferIndex) const;
	const CD3DX12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandleForHeapStart() const;
	const CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandleForHeapStart() const;

private:
	UINT descriptorSize;
	ComPtr<ID3D12DescriptorHeap> descriptorHeap;
};