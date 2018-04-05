#pragma once
// Each triangle stores the integer indices of 3 vertices
#include "Vertex.h"

struct Tri {
	int Vertex0;
	int Vertex1;
	int Vertex2;
};

class Triangle {
public:
	Triangle();
	Triangle(int v0, int v1, int v2);

	void SetVerts(int v0, int v1, int v2);
	int GetVert(int vertNum);

private:
	Tri t;
};