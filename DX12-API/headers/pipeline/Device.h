#pragma once

#include "helpers/Helpers.h"

class Resource;

class Device
{
public:
	Device(bool useWarp);

	// getters
	const ComPtr<ID3D12Device2>& Get() const;
	ID3D12Device2* GetPtr() const;

private:
	ComPtr<IDXGIAdapter4> getAdapter(bool useWarp); // encapsulate adapter logic

private:
	ComPtr<IDXGIAdapter4> adapter;
	ComPtr<ID3D12Device2> device;
};