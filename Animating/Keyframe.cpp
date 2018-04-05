#include "Keyframe.h"

Keyframe::Keyframe() {
	Time = 0;
	Value = 0;
	TangentIn = TangentOut = 0;
	RuleIn = RuleOut = rSmooth;
	A = B = C = D = 0;
}