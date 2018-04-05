#pragma once

enum rule {
	rFlat = 'f',
	rLinear = 'l',
	rSmooth = 's',
	rFixed = 'x'
};

class Keyframe{
public:
	Keyframe();

	float Time;						
	float Value;					
	float TangentIn, TangentOut;
	rule RuleIn, RuleOut;			// tangent rules
	float A, B, C, D;				// cubic coefficients
};