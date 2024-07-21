#include "PrecompiledHeaders.h"
#include "pipeline/RootSignature.h"

#include "pipeline/Device.h"

#pragma region Public Methods

RootSignature::RootSignature()
{
}

#pragma region Getters

const ComPtr<ID3D12RootSignature>& RootSignature::Get() const
{
    return rootSignature;
}

ID3D12RootSignature* RootSignature::GetPtr() const
{
    return rootSignature.Get();
}

#pragma endregion

void RootSignature::Add32BitConstant(UINT values32bitCount, UINT shaderRegister, UINT registerSpace, D3D12_SHADER_VISIBILITY shaderVisibility)
{
    CD3DX12_ROOT_PARAMETER1 rootParameter;
    rootParameter.InitAsConstants(values32bitCount, shaderRegister, registerSpace, shaderVisibility);
    rootParameters.push_back(rootParameter);
}

void RootSignature::Create(const Device& device)
{
    // check highest supported version
    D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
    featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
    if (FAILED(device.Get()->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
        featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;

    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;
    rootSignatureDescription.Init_1_1(static_cast<UINT>(rootParameters.size()), rootParameters.data(), 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    // serialize the root signature
    ComPtr<ID3DBlob> rootSignatureBlob;
    ComPtr<ID3DBlob> errorBlob;
    HRESULT hr = D3DX12SerializeVersionedRootSignature(&rootSignatureDescription,
        featureData.HighestVersion, &rootSignatureBlob, &errorBlob);

    if (FAILED(hr))
    {
        if (errorBlob)
        {
            OutputDebugStringA((char*)errorBlob->GetBufferPointer());
        }
        throw std::runtime_error("D3D12SerializeRootSignature failed!");
    }

    // create the root signature
    ThrowIfFailed(device.Get()->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(),
        rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature)));
}

#pragma endregion