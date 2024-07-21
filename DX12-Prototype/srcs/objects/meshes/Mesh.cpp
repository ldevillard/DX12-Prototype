#include "PrecompiledHeaders.h"
#include "objects/meshes/Mesh.h"

#include "helpers/Time.h"
#include "pipeline/CommandList.h"
#include "rendering/Sample.h"

#pragma region Public Methods

Mesh::Mesh(const std::vector<Vertex>& _vertices, const std::vector<WORD>& _indices)
	: vertices(_vertices)
	, indices(_indices)
    , modelMatrix(DirectX::XMMatrixIdentity())
{
}

void Mesh::OnUpdate()
{
    float angle = static_cast<float>(Time::GetTimeElapsed() * 45);
    const Vector rotationAxis = DirectX::XMVectorSet(0, 1, 1, 0);
    modelMatrix = DirectX::XMMatrixRotationAxis(rotationAxis, DirectX::XMConvertToRadians(angle));
}

void Mesh::OnRender(CommandList& commandList, int index) const
{    
    Matrix4 modelMat = modelMatrix;

    if (index == 1)
        modelMat = DirectX::XMMatrixMultiply(modelMatrix, DirectX::XMMatrixTranslation(-6, 0, 0));
    if (index == 2)
        modelMat = DirectX::XMMatrixMultiply(modelMatrix, DirectX::XMMatrixTranslation(6, 0, 0));

    // send buffers and primitive topology to the input assembler stage
    commandList.PrepareInputAssemblerStage(vertexBuffer, indexBuffer);

    // then bind model matrix and draw
    commandList.Get()->SetGraphicsRoot32BitConstants(0, sizeof(Matrix4) / 4, &modelMat, 0);
    commandList.Get()->DrawIndexedInstanced(static_cast<UINT>(indices.size()), 1, 0, 0, 0);
}

void Mesh::UpdateBuffersResource(const Sample& sample)
{
    // upload vertex buffer data
    sample.UpdateBufferResource(vertexBuffer, intermediateVertexBuffer, vertices.size(), sizeof(Vertex), vertices.data());
    // create the vertex buffer view
    vertexBuffer.CreateView(vertices.size(), sizeof(Vertex));

    // upload index buffer data
    sample.UpdateBufferResource(indexBuffer, intermediateIndexBuffer, indices.size(), sizeof(WORD), indices.data());
    // Create index buffer view
    indexBuffer.CreateView(indices.size(), sizeof(WORD), DXGI_FORMAT_R16_UINT);
}

#pragma endregion