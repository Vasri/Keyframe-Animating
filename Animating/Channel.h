#pragma once

#include "Keyframe.h"
#include "Tokenizer.h"
#include "Core.h"
#include <math.h>

enum mode {
	mConstant = 'c', 
	mLinear = 'l', 
	mCycle = 'y', 
	mCycle_offset = 't', 
	mBounce = 'b'
};

class Channel {
public: 
	Channel();
	Channel(float start, float end);

	bool Load(Tokenizer &token);
	void Precompute();
	int BinarySearch(float time, int low, int high);
	float Evaluate(float time);

private:
	float start, end;

	mode ExtrapolateIn, ExtrapolateOut;

	int numkeys;
	std::vector<Keyframe> keys;
};