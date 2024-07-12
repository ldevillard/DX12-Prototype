#pragma once

#include "helpers/Helpers.h"

class Device
{
public:
	Device(bool useWarp);

	const ComPtr<ID3D12Device2>& Get() const;

private:
	void enableDebugLayer();
	ComPtr<IDXGIAdapter4> getAdapter(bool useWarp);

private:
	ComPtr<IDXGIAdapter4> adapter;
	ComPtr<ID3D12Device2> device;
};