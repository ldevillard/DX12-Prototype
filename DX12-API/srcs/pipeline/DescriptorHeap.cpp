#include "PrecompiledHeaders.h"
#include "pipeline/DescriptorHeap.h"

#pragma region Public Methods

DescriptorHeap::DescriptorHeap(const Device& device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors, D3D12_DESCRIPTOR_HEAP_FLAGS flags)
{
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.NumDescriptors = numDescriptors;
    desc.Type = type;
    desc.Flags = flags;

    ThrowIfFailed(device.Get()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptorHeap)));

    RTVDescriptorSize = device.Get()->GetDescriptorHandleIncrementSize(type);
}

const ComPtr<ID3D12DescriptorHeap>& DescriptorHeap::Get() const
{
    return descriptorHeap;
}

ID3D12DescriptorHeap* DescriptorHeap::GetPtr() const
{
    return descriptorHeap.Get();
}

const UINT DescriptorHeap::GetRTVDescriptorSize() const
{
    return RTVDescriptorSize;
}

const CD3DX12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetRenderTargetView(int currentBackBufferIndex) const
{
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(descriptorHeap->GetCPUDescriptorHandleForHeapStart(),
        currentBackBufferIndex, RTVDescriptorSize);
}

const CD3DX12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetCPUDescriptorHandleForHeapStart() const
{
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(descriptorHeap->GetCPUDescriptorHandleForHeapStart());
}

const CD3DX12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetGPUDescriptorHandleForHeapStart() const
{
    return CD3DX12_GPU_DESCRIPTOR_HANDLE(descriptorHeap->GetGPUDescriptorHandleForHeapStart());
}

#pragma endregion