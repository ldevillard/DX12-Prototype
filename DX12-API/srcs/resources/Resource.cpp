#include "PrecompiledHeaders.h"
#include "resources/Resource.h"

#pragma region Public Methods

Resource::Resource(const ComPtr<ID3D12Resource>& res)
	: resource(res)
{
}

const ComPtr<ID3D12Resource>& Resource::Get() const
{
	return resource;
}

ID3D12Resource* Resource::GetPtr() const
{
	return resource.Get();
}

#pragma endregion