#include "Channel.h"

Channel::Channel() {
	start = end = 0;
}

Channel::Channel(float start, float end) {
	this->start = start;
	this->end = end;
}

bool Channel::Load(Tokenizer &token) {
	token.FindToken("extrapolate");

	char temp[256];
	token.GetToken(temp);
	if (strcmp(temp, "constant")==0) {
		ExtrapolateIn = mConstant;
	}
	else if (strcmp(temp, "linear")==0) {
		ExtrapolateIn = mLinear;
	}
	else if (strcmp(temp, "cycle")==0) {
		ExtrapolateIn = mCycle;
	}
	else if (strcmp(temp, "cycle_offset")==0) {
		ExtrapolateIn = mCycle_offset;
	}
	else if (strcmp(temp, "bounce")==0) {
		ExtrapolateIn = mBounce;
	}
	else {
		ExtrapolateIn = mConstant;
	}

	token.GetToken(temp);
	if (strcmp(temp, "constant")==0) {
		ExtrapolateOut = mConstant;
	}
	else if (strcmp(temp, "linear")==0) {
		ExtrapolateOut = mLinear;
	}
	else if (strcmp(temp, "cycle")==0) {
		ExtrapolateOut = mCycle;
	}
	else if (strcmp(temp, "cycle_offset")==0) {
		ExtrapolateOut = mCycle_offset;
	}
	else if (strcmp(temp, "bounce")==0) {
		ExtrapolateOut = mBounce;
	}
	else {
		ExtrapolateOut = mConstant;
	}

	token.FindToken("keys");
	numkeys = token.GetInt();
	token.FindToken("{");
	for (int i = 0; i < numkeys; i++) {
		Keyframe newKey = Keyframe();
		newKey.Time = token.GetFloat();
		newKey.Value = token.GetFloat();

		token.GetToken(temp);
		if (strcmp(temp, "flat")==0) {
			newKey.RuleIn = rFlat;
			newKey.TangentIn = 0;
		}
		else if (strcmp(temp, "linear")==0) {
			newKey.RuleIn = rLinear;
		}
		else if (strcmp(temp, "smooth")==0) {
			newKey.RuleIn = rSmooth;
		}
		else {
			newKey.RuleIn = rFixed;
			newKey.TangentIn = (float)std::atof(temp);
		}

		token.GetToken(temp);
		if (strcmp(temp, "flat")==0) {
			newKey.RuleOut = rFlat;
			newKey.TangentOut = 0;
		}
		else if (strcmp(temp, "linear")==0) {
			newKey.RuleOut = rLinear;
		}
		else if (strcmp(temp, "smooth")==0) {
			newKey.RuleOut = rSmooth;
		}
		else {
			newKey.RuleOut = rFixed;
			newKey.TangentOut = (float)std::atof(temp);
		}
		keys.push_back(newKey);
	}
		
	start = keys.front().Time;
	end = keys.back().Time;
	Precompute();
	return true;
}

// figure out TangentIn, TangentOut, A, B, C, D
void Channel::Precompute() {
	float p0, p1, t0, t1, v0, v1;

	// evaluate TangentIn, TangentOut (find slopes)
	for (int i = 1; i < numkeys - 2; i++) {
		p0 = keys[i].Value;
		t0 = keys[i].Time;
		p1 = keys[i + 1].Value;
		t1 = keys[i + 1].Time;

		// for linear tangents, just use slope formula (rise over run)
		// for smooth tangents, match the slope between the previous and next keys
		switch (keys[i].RuleOut) {
		case rLinear:
			keys[i].TangentOut = (p1 - p0) / (t1 - t0);
			break;
		case rSmooth:
			keys[i].TangentOut = (p1 - keys[i - 1].Value) / (t1 - keys[i - 1].Time);
			break;
		}

		switch (keys[i].RuleIn) {
		case rLinear:
			keys[i].TangentIn = (p0 - keys[i - 1].Value) / (t0 - keys[i - 1].Time);
			break;
		case rSmooth:
			keys[i].TangentOut = (p1 - keys[i - 1].Value) / (t1 - keys[i - 1].Time);
			break;
		}
	}

	// handle first and last keyframes
	switch (keys.front().RuleOut) {
	case rSmooth:
	case rLinear:
		keys.front().TangentOut = (keys[1].Value - keys.front().Value) / (keys[1].Time - keys.front().Time);
		break;
	}

	switch (keys.front().RuleIn) {
	case rSmooth:
	case rLinear:
		keys.front().TangentIn = keys.front().TangentOut;
	}

	switch (keys.back().RuleIn) {
	case rSmooth:
	case rLinear:
		keys.back().TangentIn = (keys.back().Value - keys[numkeys - 2].Value) / (keys.back().Time - keys[numkeys - 2].Time);
		break;
	}

	switch (keys.back().RuleOut) {
	case rSmooth:
	case rLinear:
		keys.back().TangentOut = keys.back().TangentIn;
		break;
	}

	// calculate A,B,C,D
	for (int i = 0; i < numkeys; i++) {
		p0 = keys[i].Value;
		p1 = keys[i + 1].Value;
		t0 = keys[i].Time;
		t1 = keys[i + 1].Time;
		v0 = (t1 - t0) * keys[i].TangentOut;
		v1 = (t1 - t0) * keys[i + 1].TangentIn;

		keys[i].A = (2 * p0) + (-2 * p1) + v0 + v1;
		keys[i].B = (-3 * p0) + (3 * p1) + (-2 * v0) + (-1 * v1);
		keys[i].C = v0;
		keys[i].D = p0;
	}
}

// find the keyframe at time using binarysearch. If not found, return nearest keyframe before time. 
int Channel::BinarySearch(float time, int low, int high) {
	int mid = (low + high) / 2;

	if (high > low + 1) {
		if (time > keys[mid].Time) {
			return BinarySearch(time, mid, high);
		}
		else return BinarySearch(time, low, mid);
	}

	else return low;
}

// u = invlerp(t,t0,t1) = (t - t0) / (t1 - t0)
// x = d + u(c + u(b + u(a)))
float Channel::Evaluate(float time) {
	// if there are no keys in the channel, return 0
	if (keys.empty()) {
		return 0;
	}
	// if there is just 1 key, return value of that key
	if (numkeys == 1) {
		return keys.front().Value;
	}
	
	// if time is out of range, use extrapolation rule
	// at minimum, 'constant' extrapolation rule should be used (return value of first/last key)
	float duration = end - start;
	int cycle_cnt;
	if (time < start) {
		switch (ExtrapolateIn) {
		case mConstant:
			return keys.front().Value;
		
		case mLinear:
			return keys.front().TangentIn * time;
		
		case mCycle:
			if ((start - time) > duration) {
				time = start + std::fmod(start - time, duration);
			}
			else if ((start - time) == duration) {
				return keys.back().Value;
			}
			else {
				time = start + std::fmod(duration, time);
			}
		
		case mCycle_offset:
			cycle_cnt = (int) floor((start - time) / duration);
			if ((start - time) > duration) {
				return cycle_cnt * (keys.front().Value - keys.back().Value) 
						+ Evaluate(start + std::fmod(start - time, duration));
			}
			else if ((start - time) == duration) {
				return cycle_cnt * (keys.front().Value - keys.back().Value);
			}
			else {
				return cycle_cnt * (keys.front().Value - keys.back().Value) 
						+ Evaluate(start + std::fmod(duration, time));
			}
		
		case mBounce:
			cycle_cnt = (int) floor((start - time) / duration);
			if (cycle_cnt % 2 == 0) {
				time = time - end + start - (cycle_cnt * duration);
			}
			else {
				time = end - time + start;
			}
		}
	}
	else if (time > end) {
		switch (ExtrapolateOut) {
		case mConstant:
			//printf("constant\n");
			return keys.back().Value;
		
		case mLinear:
			//printf("linear\n");
			return keys.back().TangentOut * time;
		
		case mCycle:
			//printf("cycle\n");
			time = start + std::fmod(time, duration);
		
		case mCycle_offset:
			//printf("cycleoffset\n");
			cycle_cnt = (int) floor((time - end) / duration);
			return cycle_cnt * (keys.back().Value - keys.front().Value) 
					+ Evaluate(start + std::fmod(time, duration));
		
		case mBounce:
			//printf("bounce\n");
			cycle_cnt = (int) floor((start - time) / duration);
			if (cycle_cnt % 2 == 0) {
				time = time - end + start - (cycle_cnt * duration);
			}
			else {
				time = end - time + start;
			}
		}
	}
	
	
	int idx = BinarySearch(time, 0, numkeys - 1);
	Keyframe k = keys[idx];

	// if time falls exactly on a key, just return the key value
	if (k.Time == time) {
		return k.Value;
	}

	// if there is more than one key, evaluate the curve
	// u = t-t0 / t1-t0
	float u = (time - k.Time) / (keys[idx + 1].Time - k.Time);

	// x = au3 + bu2 + cu + d = d + u(c + u(b + u(a)))
	return k.D + u * (k.C + u * (k.B + u * (k.A)));
}
