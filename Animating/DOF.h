#pragma once

#include "Core.h"

class DOF{
public:
	DOF();

	void SetValue(float val);
	void SetMin(float min);
	void SetMax(float max);
	float GetValue();
	float GetMin();
	float GetMax();
	void Increment();
	void Decrement();

private:
	float Value;
	float Min;
	float Max;
};