#include "PrecompiledHeaders.h"
#include "objects/Model.h"

#include "helpers/Time.h"
#include "objects/meshes/Cube.h"
#include "pipeline/CommandList.h"

#pragma region Public Methods

Model::Model(const std::string& modelPath)
	: transform(DirectX::XMMatrixIdentity())
{
	// mesh loader
}

Model::Model(ModelPrimitive modelPrimitive)
	: transform(DirectX::XMMatrixIdentity())
{
	switch (modelPrimitive)
	{
		case ModelPrimitive::CubePrimitive:
			meshes.push_back(Cube());
			break;
		default:
			break;
	}
}

#pragma region Setters

void Model::SetTransform(const Matrix4& _transform)
{
	transform = _transform;
}

#pragma endregion

void Model::OnInit(const Sample& sample)
{
	for (Mesh& mesh : meshes)
		mesh.OnInit(sample);
}

void Model::OnUpdate()
{
}

void Model::OnRender(CommandList& commandList)
{
	commandList.Get()->SetGraphicsRoot32BitConstants(0, sizeof(Matrix4) / 4, &transform, 0);

	for (Mesh& mesh : meshes)
		mesh.OnRender(commandList);
}

#pragma endregion