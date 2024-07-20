#include "PrecompiledHeaders.h"
#include "resources/IndexBuffer.h"

#pragma region Public Methods

IndexBuffer::IndexBuffer()
	: Buffer()
	, indiciesCount(0)
	, indexFormat(DXGI_FORMAT_R16_UINT)
	, indexBufferView()
{
}

#pragma region Getters

D3D12_INDEX_BUFFER_VIEW IndexBuffer::GetIndexBufferView() const
{
	return indexBufferView;
}

#pragma endregion

void IndexBuffer::CreateView(size_t indiciesCount, USHORT indexSize, DXGI_FORMAT indexFormat)
{
	this->indiciesCount = indiciesCount;
	this->indexFormat = indexFormat;

	indexBufferView.BufferLocation = resource->GetGPUVirtualAddress();
	indexBufferView.Format = indexFormat;
	indexBufferView.SizeInBytes = static_cast<UINT>(indiciesCount * indexSize);
}

#pragma endregion