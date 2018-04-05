#pragma once

#include "Camera.h"
#include "Shader.h"
#include "Skeleton.h"
#include "Skin.h"
#include "Animation.h"
#include "Player.h"
#include <iostream>

static const float V_AIR = 1.0f;

class KeyAnimating {
public:
	KeyAnimating(const char *windowTitle,int argc,char **argv);
	~KeyAnimating();

	void Update();
	void Reset();
	void Draw();

	void Quit();

	// Event handlers
	void Resize(int x,int y);
	void Keyboard(int key,int x,int y);
	void MouseButton(int btn,int state,int x,int y);
	void MouseMotion(int x,int y);

private:
	// Window management
	int WindowHandle;
	int WinX,WinY;

	// Input
	bool LeftDown,MiddleDown,RightDown;
	int MouseX,MouseY;

	// Interactivity (skeleton joint manipulation)
	int jointNo;
	int numJoints;
	int jointMode;

	float deltaTime;

	// Components
	ShaderProgram *Program;
	Skeleton* Skel;
	Skin* skin;
	Camera *Cam;
	Player* player;
	Animation* Clip;
};