#include "PrecompiledHeaders.h"
#include "resources/IndexBuffer.h"

#pragma region Public Methods

IndexBuffer::IndexBuffer()
	: Buffer()
	, indicesCount(0)
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

void IndexBuffer::CreateView(size_t indicesCount, USHORT indexSize, DXGI_FORMAT indexFormat)
{
	this->indicesCount = indicesCount;
	this->indexFormat = indexFormat;

	indexBufferView.BufferLocation = resource->GetGPUVirtualAddress();
	indexBufferView.Format = indexFormat;
	indexBufferView.SizeInBytes = static_cast<UINT>(indicesCount * indexSize);
}

#pragma endregion