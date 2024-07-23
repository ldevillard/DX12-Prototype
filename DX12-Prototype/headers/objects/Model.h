#pragma once

#include <string>
#include <vector>

#include "maths/Maths.h"
#include "objects/meshes/Mesh.h"

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
	std::vector<Mesh> meshes;
	Matrix4 transform;
};