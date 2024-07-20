#include "PrecompiledHeaders.h"
#include "resources/VertexBuffer.h"

#pragma region Public Methods

VertexBuffer::VertexBuffer()
	: Buffer()
	, verticesCount(0)
	, vertexSize(0)
	, vertexBufferView()
{
}

#pragma region Getters

D3D12_VERTEX_BUFFER_VIEW VertexBuffer::GetVertexBufferView() const
{
	return vertexBufferView;
}

#pragma endregion

void VertexBuffer::CreateView(size_t verticesCount, size_t vertexSize)
{
	this->verticesCount = verticesCount;
	this->vertexSize = vertexSize;

	vertexBufferView.BufferLocation = resource->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = static_cast<UINT>(verticesCount * vertexSize);
	vertexBufferView.StrideInBytes = static_cast<UINT>(vertexSize);
}

#pragma endregion