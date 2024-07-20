#include "PrecompiledHeaders.h"
#include "resources/Resource.h"

#pragma region Public Methods

Resource::Resource()
{
}

Resource::Resource(const ComPtr<ID3D12Resource>& res)
	: resource(res)
{
}

#pragma region Getters

const ComPtr<ID3D12Resource>& Resource::Get() const
{
	return resource;
}

ID3D12Resource* Resource::GetPtr() const
{
	return resource.Get();
}

#pragma endregion

#pragma region Setters

void Resource::Set(const ComPtr<ID3D12Resource> ptr)
{
	resource = ptr;
}

#pragma endregion

void Resource::Reset()
{
	resource.Reset();
}

#pragma endregion