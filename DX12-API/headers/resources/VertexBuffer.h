#pragma once

#include "resources/Buffer.h"

class VertexBuffer : public Buffer
{
public:
	VertexBuffer();

	void CreateView(size_t verticesCount, size_t vertexSize);
	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const;

private:
	size_t verticesCount;
	size_t vertexSize;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
};