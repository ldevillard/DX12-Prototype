#pragma once

#include <vector>
#include "helpers/Helpers.h"

class Device;

class RootSignature
{
public:
	RootSignature();

	const ComPtr<ID3D12RootSignature>& Get() const;
	ID3D12RootSignature* GetPtr() const;

	void Add32BitConstant(UINT values32bitCount, UINT shaderRegister, UINT registerSpace = 0U, D3D12_SHADER_VISIBILITY shaderVisibility = D3D12_SHADER_VISIBILITY_ALL);
	// TODO: add CBV, SRV, UAV methods...

	void Create(const Device& device);

private:
	std::vector<CD3DX12_ROOT_PARAMETER1> rootParameters;
	ComPtr<ID3D12RootSignature> rootSignature;
};