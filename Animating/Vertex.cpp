#include "Vertex.h"

Vertex::Vertex() {
	Position = vec3(0.0f);
	Normal = vec3(0.0f);
}

Vertex::Vertex(vec3 pos, vec3 norm) {
	Position = pos;
	Normal = norm;
}

void Vertex::SetPos(vec3 newPos) {
	Position = newPos;
}

void Vertex::SetNorm(vec3 newNorm) {
	Normal = newNorm;
}

vec3 Vertex::GetPos() {
	return Position;
}

vec3 Vertex::GetNorm() {
	return Normal;
}