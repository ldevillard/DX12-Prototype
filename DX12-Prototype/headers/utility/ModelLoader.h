#pragma once

#include <string>
#include <vector>

// assimp
struct aiMesh;
struct aiNode;
struct aiScene;

class Mesh;

class ModelLoader
{
public:
	ModelLoader() = default;

	void Load(const std::string& modelPath, std::vector<Mesh>& outMeshes);

private:
	void processNode(aiNode* node, const aiScene* scene, std::vector<Mesh>& outMeshes);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
};