#include "Triangle.h"

Triangle::Triangle() {
	t.Vertex0 = t.Vertex1 = t.Vertex2 = 0;
}

Triangle::Triangle(int v0, int v1, int v2) {
	t.Vertex0 = v0;
	t.Vertex1 = v1;
	t.Vertex2 = v2;
}

void Triangle::SetVerts(int v0, int v1, int v2) {
	t.Vertex0 = v0;
	t.Vertex1 = v1;
	t.Vertex2 = v2;
}

int Triangle::GetVert(int vertNum) {
	switch (vertNum) {
	
	case 0:
		return t.Vertex0;
	case 1:
		return t.Vertex1;
	case 2:
		return t.Vertex2;
	default:
		return 0;
	}
}
