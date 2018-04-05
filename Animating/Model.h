////////////////////////////////////////
// Model.h
////////////////////////////////////////

#pragma once

#include "Core.h"
#include "Vertex.h"
#include "Triangle.h"

////////////////////////////////////////////////////////////////////////////////

struct ModelVertex {
	glm::vec3 Position;
	glm::vec3 Normal;
};

////////////////////////////////////////////////////////////////////////////////

class Model {
public:
	Model();
	~Model();

	void Draw(const glm::mat4 &modelMtx,const glm::mat4 &viewProjMtx,uint shader);

	void MakeBox(const glm::vec3 &boxMin,const glm::vec3 &boxMax);
	void SetBuffers(const std::vector<ModelVertex> &vtx,const std::vector<uint> &idx);

	void MakeTriangle(Vertex v0, Vertex v1, Vertex v2);
	void MakeSkin(std::vector<Vertex> vertices, std::vector<Triangle> indices);

	// Access functions

private:
	uint VertexBuffer;
	uint IndexBuffer;

	int Count;
};

////////////////////////////////////////////////////////////////////////////////
