#include "PrecompiledHeaders.h"
#include "objects/Model.h"

// assimp
#include <Importer.hpp>
#include <scene.h>
#include <postprocess.h>

#include "helpers/Time.h"
#include "objects/meshes/Cube.h"
#include "pipeline/CommandList.h"

#pragma region Public Methods

Model::Model(const std::string& modelPath)
	: transform(DirectX::XMMatrixIdentity())
{
	// TODO: MeshLoader class
	loadModel(modelPath);
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

#pragma region Private Methods

void Model::loadModel(std::string modelPath)
{
	Assimp::Importer import;
	const aiScene * scene = import.ReadFile(modelPath, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::string errorMessage("ERROR::ASSIMP::" + std::string(import.GetErrorString()) + "\n");
		OutputDebugStringA(errorMessage.c_str());
		return;
	}
	//directory = path.substr(0, path.find_last_of('/'));

	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	// process all the node's meshes (if any)
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}

	// then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
		processNode(node->mChildren[i], scene);
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    // data to fill
    std::vector<Vertex> vertices;
    std::vector<WORD> indices;

    // walk through each of the mesh's vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex = {};
        // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
        Float3 vector = {};
        
        // positions
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;
        
        // normals
        if (mesh->HasNormals())
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
        }

        vertices.push_back(vertex);
    }
    // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    
    // return a mesh object created from the extracted mesh data
    return Mesh(vertices, indices);
}

#pragma endregion