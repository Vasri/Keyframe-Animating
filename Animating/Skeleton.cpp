#include "Skeleton.h"

bool Skeleton::Load(const char* file){
	Tokenizer token;
	printf("loading %s\n",file);
	token.Open(file);
	token.FindToken("balljoint");

	//Parse tree
	Root = new Joint;
	Root->Load(token);
	Root->dfs(jointTree);

	//Finish
	token.Close();
	return true;
}

mat4 Skeleton::GetWorldMatrix(int jointNo) {
	
	if (jointTree.empty()) {
		return mat4(1.0f);
	}
	else {
		return jointTree[jointNo]->GetMatrix();
	}
}

int Skeleton::getTreeSize() {
	return jointTree.size();
}

void Skeleton::Update() {
	// traverses tree and computes joint matrices
	Root->Update();
}

void Skeleton::Draw(const mat4 &viewProjMat, uint shader) {
	Root->Draw(viewProjMat, shader);
}