#pragma once

#include "resources/Buffer.h"

class IndexBuffer : public Buffer
{
public:
	IndexBuffer();

	// getters
	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const;

	void CreateView(size_t indicesCount, USHORT indexSize, DXGI_FORMAT indexFormat);

private:
	size_t indicesCount;
	DXGI_FORMAT indexFormat;

	D3D12_INDEX_BUFFER_VIEW indexBufferView;
};