#pragma once

#include "helpers/Helpers.h"

class Device;

class PipelineStateObject
{
public:
	PipelineStateObject(const Device& device, D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDescriptor);

	const ComPtr<ID3D12PipelineState>& Get() const;
	ID3D12PipelineState* GetPtr() const;

private:
	ComPtr<ID3D12PipelineState> pipelineStateObject;
};