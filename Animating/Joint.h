#pragma once

#include "DOF.h"
#include "Model.h"
#include "Tokenizer.h"

class Joint{
public:
	std::vector<DOF*> DOFs;

	Joint();
	~Joint();
	bool Load(Tokenizer &t);
	void AddChild(Joint* newChild);
	void Update();
	void Draw(const mat4 &viewProjMat, uint shader);
	bool dfs(std::vector<Joint*> &jointTree);
	mat4 GetMatrix();
	bool updateOffset(float x, float y, float z);

private:
	// Offset, Boxmin, Boxmax are used in drawing
	vec3 Offset;
	vec3 BoxMin;
	vec3 BoxMax;

	mat4 trans;
	mat4 rotx;
	mat4 roty;
	mat4 rotz;

	char* name;

	mat4 LocalMat;
	mat4 WorldMat;
	Model Box;

	bool visited;	// for dfs

	Joint* parent;
	std::vector<Joint*> children;
};