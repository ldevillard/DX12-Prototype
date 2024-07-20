#pragma once

#include "helpers/Helpers.h"

class Resource
{
public:
	Resource();
	Resource(const ComPtr<ID3D12Resource>& res);

	// getters
	const ComPtr<ID3D12Resource>& Get() const;
	ID3D12Resource* GetPtr() const;

	// setters
	void Set(const ComPtr<ID3D12Resource> ptr);

	void Reset();

protected:
	ComPtr<ID3D12Resource> resource;
};