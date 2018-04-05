#include "DOF.h"

DOF::DOF(){
	Value = 0;
	Min = -100000;
	Max = 100000;
}

/* set the value of this DOF. Clamp if out of range*/
void DOF::SetValue(float val){
	Value = glm::clamp(val, Min, Max);
}

void DOF::SetMin(float min){
	Min = min;
}

void DOF::SetMax(float max){
	Max = max;
}

float DOF::GetValue(){
	return Value;
}

float DOF::GetMin(){
	return Min;
}

float DOF::GetMax(){
	return Max;
}

void DOF::Increment() {
	Value = glm::clamp(Value+0.05f, Min, Max);
}

void DOF::Decrement() {
	Value = glm::clamp(Value-0.05f, Min, Max);
}