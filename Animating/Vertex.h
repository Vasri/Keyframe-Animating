#pragma once

#include "Core.h"
// Each vertex has its x,y,z values plus the vertex normal

class Vertex {
public:
	Vertex();
	Vertex(vec3 pos, vec3 norm);

	void SetPos(vec3 newPos);
	void SetNorm(vec3 newNorm);
	vec3 GetPos();
	vec3 GetNorm();
	
private:
	vec3 Position;
	vec3 Normal;
};