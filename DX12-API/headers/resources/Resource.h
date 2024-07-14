#pragma once

#include "pipeline/Device.h"

class Resource
{
public:
	Resource() = default;
	Resource(const ComPtr<ID3D12Resource>& res);

	const ComPtr<ID3D12Resource>& Get() const;
	ID3D12Resource* GetPtr() const;

	void Reset();

private :
	ComPtr<ID3D12Resource> resource;
};