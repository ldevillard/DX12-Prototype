#pragma once

#include <vector>

#include "data/geometry/Vertex.h"
#include "resources/IndexBuffer.h"
#include "resources/VertexBuffer.h"

class CommandList;
class Sample;

class Mesh
{
public:
	Mesh() = default;
	Mesh(const std::vector<Vertex>& vertices, const std::vector<WORD>& indices);

	void OnRender(CommandList& commandList, int index = 0) const;

	void UpdateBuffersResource(const Sample& sample);

private:
	IndexBuffer indexBuffer;
	VertexBuffer vertexBuffer;

	// TODO: make a destruction of these ressources after init
	Resource intermediateVertexBuffer;
	Resource intermediateIndexBuffer;

	std::vector<Vertex> vertices;
	std::vector<WORD> indices;
};