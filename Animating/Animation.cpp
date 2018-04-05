#include "Animation.h"

Animation::Animation() {
	start = end = 0;
}

bool Animation::Load(const char* filename) {
	Tokenizer token;
	token.Open(filename);

	token.FindToken("range");
	start = token.GetFloat();
	end = token.GetFloat();

	token.FindToken("numchannels");
	int numchannels = token.GetInt();

	for (int i = 0; i < numchannels; i++) {
		Channel* newChannel = new Channel(start, end);

		token.FindToken("channel");
		newChannel->Load(token);
		channels.push_back(newChannel);
	}
	token.Close();
	return true;
}

void Animation::Evaluate(float time, std::vector<float> &pose) {
	pose.clear();

	for (auto& c : channels) {
		float x = c->Evaluate(time);
		pose.push_back(x);
	}
}

float Animation::getStart() {
	return start;
}

float Animation::getEnd() {
	return end;
}