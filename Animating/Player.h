#pragma once
#include "Animation.h"
#include "Skeleton.h"

class Player {
public:
	Player();

	void setClip(Animation* clip);
	void setSkel(Skeleton* skel);
	const std::vector<float> &GetPose();
	void Update();
	void Reset();


private:
	Animation* anim;
	Skeleton* skel;

	float time;
	std::vector<float> pose;
};