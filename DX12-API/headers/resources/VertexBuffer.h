#pragma once

#include "resources/Buffer.h"

class VertexBuffer : public Buffer
{
public:
	VertexBuffer();

	// getters
	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const;
	
	void CreateView(size_t verticesCount, size_t vertexSize);

private:
	size_t verticesCount;
	size_t vertexSize;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
};