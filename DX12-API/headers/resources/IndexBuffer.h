#pragma once

#include "resources/Buffer.h"

class IndexBuffer : public Buffer
{
public:
	IndexBuffer();

	void CreateView(size_t indiciesCount, USHORT indexSize, DXGI_FORMAT indexFormat);
	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const;

private:
	size_t indiciesCount;
	DXGI_FORMAT indexFormat;

	D3D12_INDEX_BUFFER_VIEW indexBufferView;
};