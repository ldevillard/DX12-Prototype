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
	Mesh(const std::vector<Vertex>& vertices, const std::vector<UINT>& indices);

	void OnInit(const Sample& sample);
	void OnRender(CommandList& commandList, int index = 0) const;

private:
	void updateBuffersResource(const Sample& sample);

protected:
	Mesh() = default;

protected:
	std::vector<Vertex> vertices;
	std::vector<UINT> indices;

private:
	IndexBuffer indexBuffer;
	VertexBuffer vertexBuffer;

	// TODO: maybe make a destruction of these ressources after init
	Resource intermediateVertexBuffer;
	Resource intermediateIndexBuffer;
};