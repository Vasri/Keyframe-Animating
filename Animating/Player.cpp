#include "Player.h"

Player::Player() {
	anim = new Animation();
	skel = new Skeleton();
	time = 0.0f;
}

void Player::setClip(Animation* clip) {
	anim = clip;
}

void Player::setSkel(Skeleton* skel) {
	this->skel = skel;
}

const std::vector<float> &Player::GetPose() {
	return this->pose;
}

void Player::Update() {
	time = time + 0.001f;

	anim->Evaluate(time, pose);

	skel->jointTree[0]->updateOffset(pose[0], pose[1], pose[2]);

	for (uint i = 0, j = 1; i < skel->jointTree.size(); i++, j++) {
		skel->jointTree[i]->DOFs[0]->SetValue(pose[3 * j]);
		skel->jointTree[i]->DOFs[1]->SetValue(pose[3 * j + 1]);
		skel->jointTree[i]->DOFs[2]->SetValue(pose[3 * j + 2]);
	}
}

void Player::Reset() {
	time = 0.0;
}