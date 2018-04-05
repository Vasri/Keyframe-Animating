#include "Skin.h"

Skin::Skin() {
}

Skin::Skin(Skeleton* s) {
	skel = s;
}

bool Skin::Load(const char* fileName) {
	Tokenizer token;
	token.Open(fileName);

	if (token.FindToken("positions")) {
		fprintf(stderr, "Getting positions\n");
		vec3 position;
		int numverts = token.GetInt();

		token.FindToken("{");
		// add numverts amount of new vertices to Vertices vector
		for (int i = 0; i < numverts; i++) {
			position.x = token.GetFloat();
			position.y = token.GetFloat();
			position.z = token.GetFloat();

			Vertices.push_back(Vertex());
			Vertices[i].SetPos(position);
			
			// toDraw array stores vertices to be drawn (calculated in Update())
			toDraw.push_back(Vertex());
		}
	}
	else return false;
	
	if (token.FindToken("normals")) {
		fprintf(stderr, "Getting normals\n");
		vec3 normal;
		int numverts = token.GetInt();

		token.FindToken("{");
		// add numverts amount of normals to vertices in Vertices vector
		for (int i = 0; i < numverts; i++) {
			normal.x = token.GetFloat();
			normal.y = token.GetFloat();
			normal.z = token.GetFloat();
			Vertices[i].SetNorm(normal);
		}
	}
	else return false;

	if (token.FindToken("skinweights")) {
		fprintf(stderr, "Getting skinweights\n");
		int numverts = token.GetInt();

		token.FindToken("{");
		// each vertex in Vertices vector has corresponding weight(s) in Weights vector
		for (int i = 0; i < numverts; i++) {

			// the Weights vector holds a vector of weights for each vertex
			std::vector<SkinWeight> weights;
			
			int numattachments = token.GetInt();
			for (int j = 0; j < numattachments; j++) {
				weights.push_back({ token.GetInt(), token.GetFloat() });
			}
			Weights.push_back(weights);
		}
	}
	else return false;

	if (token.FindToken("triangles")) {
		fprintf(stderr, "Getting triangles\n");
		int numtriangles = token.GetInt();

		token.FindToken("{");
		// add numtriangles new triangles to Triangles vector
		for (int i = 0; i < numtriangles; i++) {
			Triangle newTri = Triangle();
			newTri.SetVerts(token.GetInt(), token.GetInt(), token.GetInt());
			Triangles.push_back(newTri);
		}
	}
	else return false;

	if (token.FindToken("bindings")) {
		fprintf(stderr, "Getting bindings\n");
		int numjoints = token.GetInt();

		token.FindToken("{");

		for (int i = 0; i < numjoints; i++) {
			mat4 matrix;
			token.FindToken("{");

			float ax = token.GetFloat();
			float ay = token.GetFloat();
			float az = token.GetFloat();

			float bx = token.GetFloat();
			float by = token.GetFloat();
			float bz = token.GetFloat();

			float cx = token.GetFloat();
			float cy = token.GetFloat();
			float cz = token.GetFloat();

			float dx = token.GetFloat();
			float dy = token.GetFloat();
			float dz = token.GetFloat();

			matrix = mat4(ax, ay, az, 0.0f,
						bx, by, bz, 0.0f,
						cx, cy, cz, 0.0f,
						dx, dy, dz, 1.0f);
			
			Bindings.push_back(inverse(matrix));
		}
	}
	else return false;
	
	token.Close();
	fprintf(stderr, "done\n\n");
	return true;
}

void Skin::Update() {
	SkinMat.clear();

	// 'binding' coords to local coords with bind matrix
	// basically, do W * B^-1
	int i = 0;
	for (auto& b : Bindings) {
		SkinMat.push_back(skel->GetWorldMatrix(i) * b);
		i++;
	}

	// apply transformation to vertex positions and normals
	// position is a weighted sum; normal is a normalized weighted sum
	i = 0;
	for (auto& v : Vertices) {
		vec4 sumPos = vec4(0.0f);
		vec4 sumNorm = vec4(0.0f);
		for (auto& w : Weights[i]) {
			// position vector has a 1 as 4th member; normal has 0
			vec4 pos = vec4(v.GetPos(),1.0f);
			vec4 norm = vec4(v.GetNorm(),0.0f);

			sumPos = sumPos + w.weight * SkinMat[w.joint] * pos;
			sumNorm = sumNorm + w.weight * SkinMat[w.joint] * norm;
		}
		sumNorm = normalize(sumNorm);

		toDraw[i].SetPos(sumPos);
		toDraw[i].SetNorm(sumNorm);
		
		i++;
	}

	skin.MakeSkin(toDraw, Triangles);
	SkinMat.clear();
}

void Skin::Draw(const mat4 &viewProjMat, uint shader) {
	skin.Draw(mat4(1.0f), viewProjMat, shader);
}

Skin::~Skin() {
	skel = NULL;
}