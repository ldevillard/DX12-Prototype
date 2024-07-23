#include "PrecompiledHeaders.h"
#include "utility/ModelLoader.h"

// assimp
#include <Importer.hpp>
#include <scene.h>
#include <postprocess.h>

#include "objects/meshes/Mesh.h"

#pragma region Public Methods

void ModelLoader::Load(const std::string& modelPath, std::vector<Mesh>& outMeshes)
{
	Assimp::Importer import;
	const aiScene * scene = import.ReadFile(modelPath, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::string errorMessage("ERROR::ASSIMP::" + std::string(import.GetErrorString()) + "\n");
		OutputDebugStringA(errorMessage.c_str());
		return;
	}
	processNode(scene->mRootNode, scene, outMeshes);
}

#pragma endregion

#pragma region Private Methods

void ModelLoader::processNode(aiNode* node, const aiScene* scene, std::vector<Mesh>& outMeshes)
{
	// process all the node's meshes (if any)
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		outMeshes.push_back(processMesh(mesh, scene));
	}

	// then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
		processNode(node->mChildren[i], scene, outMeshes);
}

Mesh ModelLoader::processMesh(aiMesh* mesh, const aiScene* scene)
{
    // data to fill
    std::vector<Vertex> vertices;
    std::vector<UINT> indices;

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

        vertex.Color = Float3(1, 0.5f, 0.5f);
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