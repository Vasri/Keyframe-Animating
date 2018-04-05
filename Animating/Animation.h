#pragma once
#include "Core.h"
#include "Channel.h"

class Animation {
public:
	Animation();

	bool Load(const char *filename);
	void Evaluate(float time, std::vector<float> &pose);
	float getStart();
	float getEnd();

private:
	float start;
	float end;
	std::vector<Channel*> channels;
};