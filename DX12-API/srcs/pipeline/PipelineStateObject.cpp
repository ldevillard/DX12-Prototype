#include "PrecompiledHeaders.h"
#include "pipeline/PipelineStateObject.h"

#include "pipeline/Device.h"

#pragma region Public Methods

PipelineStateObject::PipelineStateObject(const Device& device, D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDescriptor)
{
	ThrowIfFailed(device.Get()->CreateGraphicsPipelineState(&psoDescriptor, IID_PPV_ARGS(&pipelineStateObject)));
}

const ComPtr<ID3D12PipelineState>& PipelineStateObject::Get() const
{
	return pipelineStateObject;
}

ID3D12PipelineState* PipelineStateObject::GetPtr() const
{
	return pipelineStateObject.Get();
}

#pragma endregion