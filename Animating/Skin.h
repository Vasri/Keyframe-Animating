#pragma once

#include "Triangle.h"
#include "Tokenizer.h"
#include "Skeleton.h"

struct SkinWeight {
	int joint;
	float weight;
};

class Skin {
public:
	Skin();
	Skin(Skeleton* s);
	~Skin();
	bool Load(const char* fileName);
	void Update();
	void Draw(const mat4 &viewProjMat, uint shader);

private:
	std::vector<Vertex> Vertices;	// each vertex holds a float tuple of positions, and a float tuple of normals
	std::vector<std::vector<SkinWeight>> Weights;
	std::vector<Triangle> Triangles; // each triangle holds indices to 3 vertices
	std::vector<mat4> Bindings;
	
	std::vector<mat4> SkinMat;	// temp variable to help with update calculations
	std::vector<Vertex> toDraw;	// final vertices to be drawn
	
	Skeleton* skel;
	Model skin;
};