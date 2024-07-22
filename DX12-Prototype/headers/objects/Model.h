#pragma once

#include <string>
#include <vector>

#include "maths/Maths.h"
#include "objects/meshes/Mesh.h"

struct aiMesh;
struct aiNode;
struct aiScene;

class CommandList;
class Sample;

enum ModelPrimitive
{
	SpherePrimitive,
	CubePrimitive
	//...
};

class Model
{
public:
	Model(const std::string& modelPath);
	Model(ModelPrimitive modelPrimitive);

	void SetTransform(const Matrix4& transform);

	void OnInit(const Sample& sample);
	void OnUpdate();
	void OnRender(CommandList& commandList);

private:
	void loadModel(std::string modelPath);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);

private:
	std::vector<Mesh> meshes;
	Matrix4 transform;
};