#include "Joint.h"

Joint::Joint(){
	trans = mat4(1.0f);
	rotx = mat4(1.0f);
	roty = mat4(1.0f);
	rotz = mat4(1.0f);

	LocalMat = mat4(1.0f);
	WorldMat = mat4(1.0f);
	Joint* parent = NULL;

	visited = false;

	// each Joint has x,y,z rotational axes by default
	DOF* x = new DOF();
	DOF* y = new DOF();
	DOF* z = new DOF();

	DOFs.push_back(x);
	DOFs.push_back(y);
	DOFs.push_back(z);

	Offset = vec3(0.0f);

	BoxMin = vec3(-0.1f);
	BoxMax = vec3(0.1f);
}

bool Joint::Load(Tokenizer &token){
	token.FindToken("{");
	while(1){
		char temp[256];
		token.GetToken(temp);
		if(strcmp(temp,"offset")==0){
			Offset.x = token.GetFloat();
			Offset.y = token.GetFloat();
			Offset.z = token.GetFloat();
			//fprintf(stderr,"set Offset %f,%f,%f\n", Offset.x, Offset.y, Offset.z);
		}
		// check other tokens
		else if(strcmp(temp, "boxmin")==0){ 
			BoxMin.x = token.GetFloat();
			BoxMin.y = token.GetFloat();
			BoxMin.z = token.GetFloat();
			//fprintf(stderr,"set BoxMin %f,%f,%f\n", BoxMin.x, BoxMin.y, BoxMin.z);
		}
		else if(strcmp(temp, "boxmax")==0){
			BoxMax.x = token.GetFloat();
			BoxMax.y = token.GetFloat();
			BoxMax.z = token.GetFloat();
			//fprintf(stderr,"set BoxMax %f,%f,%f\n", BoxMax.x, BoxMax.y, BoxMax.z);
		}
		else if(strcmp(temp, "rotxlimit")==0){ 
			DOFs[0]->SetMin(token.GetFloat());
			DOFs[0]->SetMax(token.GetFloat());
		}
		else if(strcmp(temp, "rotylimit")==0){ 
			DOFs[1]->SetMin(token.GetFloat());
			DOFs[1]->SetMax(token.GetFloat());
		}
		else if(strcmp(temp, "rotzlimit")==0){ 
			DOFs[2]->SetMin(token.GetFloat());
			DOFs[2]->SetMax(token.GetFloat());
		}
		else if(strcmp(temp, "pose")==0){
			float xRot = token.GetFloat();
			float yRot = token.GetFloat();
			float zRot = token.GetFloat();
			DOFs[0]->SetValue(xRot);
			DOFs[1]->SetValue(yRot);
			DOFs[2]->SetValue(zRot);
			//fprintf(stderr,"posing %f, %f, %f\n", DOFs[0]->GetValue(), DOFs[1]->GetValue(), DOFs[2]->GetValue());
		}
		// possibly add more subjoints in future. For now, we only have balljoints
		else if(strcmp(temp, "balljoint")==0){
			token.GetToken(temp);
			name = temp;

			fprintf(stderr,"\n\n%s:\n",name);
			Joint* jnt = new Joint();
			jnt->Load(token);
			AddChild(jnt);
		}
		else if(strcmp(temp,"}")==0) {
			Update();
			return true;
		}
		else{
			token.SkipLine();	// unrecognized token
		}
	}
}

// A child joint's local matrix should be based on the parent joint's world matrix
void Joint::AddChild(Joint* newChild){
	newChild->parent = this;
	children.push_back(newChild);
	//jointTree.push_back(newChild);
}

void Joint::Update(){
	// since Update is done recursively, LocalMat should be kept up to date
	if (parent != NULL) {
		LocalMat = parent->LocalMat * parent->WorldMat;
	}

	// translation matrix comes from offset
	trans[3].x = Offset.x;
	trans[3].y = Offset.y;
	trans[3].z = Offset.z;

	// rotation matrices come from DOFs 0,1,2 (x,y,z axes respectively)
	float theta = DOFs[0]->GetValue();
	rotx = mat4(1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, cos(theta), sin(theta), 0.0f, 
				0.0f, -1.0f*sin(theta), cos(theta), 0.0f, 
				0.0f, 0.0f, 0.0f, 1.0f);

	theta = DOFs[1]->GetValue();
	roty = mat4(cos(theta), 0.0f, -1.0f*sin(theta), 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				sin(theta), 0.0f, cos(theta), 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f);

	theta = DOFs[2]->GetValue();
	rotz = mat4(cos(theta), sin(theta), 0.0f, 0.0f,
				-1.0f*sin(theta), cos(theta), 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f, 
				0.0f, 0.0f, 0.0f, 1.0f);

	WorldMat = trans * rotz * roty * rotx;
	Box.MakeBox(BoxMin,BoxMax);

	// recursively call Update() on children
	for(std::vector<Joint*>::iterator it = children.begin(); it != children.end(); it++) {
		(*it)->Update();
	}
}

bool Joint::updateOffset(float x, float y, float z) {
	Offset = vec3(x, y, z);
	return true;
}

// drawing should also be done recursively
void Joint::Draw(const mat4 &viewProjMat, uint shader){

	Box.Draw(LocalMat*WorldMat, viewProjMat, shader);

	for(std::vector<Joint*>::iterator it = children.begin(); it != children.end(); it++) {
		(*it)->Draw(viewProjMat, shader);
	}
}

// depth first search to build joint tree in skeleton
bool Joint::dfs(std::vector<Joint*> &jointTree) {
	visited = true;

	jointTree.push_back(this);

	for (Joint* c : children) {
		if (c->visited == false) c->dfs(jointTree);
	}
	return true;
}

// necessary to transform skin from binding coordinates to joint coordinates
mat4 Joint::GetMatrix() {
	return LocalMat * WorldMat;
}

Joint::~Joint(){
	delete parent;
	delete name;
}