#pragma once

#include "pipeline/Device.h"

class DescriptorHeap
{
public:
	DescriptorHeap(const Device& device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors);

	const ComPtr<ID3D12DescriptorHeap>& Get() const;
	const UINT GetRTVDescriptorSize() const;

private:
	UINT RTVDescriptorSize;
	ComPtr<ID3D12DescriptorHeap> descriptorHeap;
};