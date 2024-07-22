#include "PrecompiledHeaders.h"
#include "objects/meshes/Mesh.h"

#include "helpers/Time.h"
#include "pipeline/CommandList.h"
#include "rendering/Sample.h"

#pragma region Public Methods

Mesh::Mesh(const std::vector<Vertex>& _vertices, const std::vector<WORD>& _indices)
	: vertices(_vertices)
	, indices(_indices)
{
}

void Mesh::OnInit(const Sample& sample)
{
    updateBuffersResource(sample);
}

void Mesh::OnRender(CommandList& commandList, int index) const
{    
    // send buffers and primitive topology to the input assembler stage
    commandList.PrepareInputAssemblerStage(vertexBuffer, indexBuffer);
    commandList.Get()->DrawIndexedInstanced(static_cast<UINT>(indices.size()), 1, 0, 0, 0);
}

#pragma endregion

#pragma region Private Methods

void Mesh::updateBuffersResource(const Sample& sample)
{
    // upload vertex buffer data
    sample.UpdateBufferResource(vertexBuffer, intermediateVertexBuffer, vertices.size(), sizeof(Vertex), vertices.data());
    // create the vertex buffer view
    vertexBuffer.CreateView(vertices.size(), sizeof(Vertex));

    // upload index buffer data
    sample.UpdateBufferResource(indexBuffer, intermediateIndexBuffer, indices.size(), sizeof(WORD), indices.data());
    // create index buffer view
    indexBuffer.CreateView(indices.size(), sizeof(WORD), DXGI_FORMAT_R16_UINT);
}

#pragma endregion