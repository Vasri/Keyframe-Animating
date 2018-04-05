#pragma once

#include "Joint.h"

class Skeleton{
public:
	std::vector<Joint*> jointTree;

	bool Load(const char* file);
	void Update();
	void Draw(const mat4 &viewProjMat, uint shader);
	mat4 GetWorldMatrix(int jointNo);
	int getTreeSize();

private:
	Joint* Root;
};