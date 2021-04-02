#pragma once
#include "MoveComponent.h"
#include <cstdint>

class InputComponent : public MoveComponent
{
public:
	InputComponent(class Actor* owner);
	void ProcessInput(const uint8_t* keyState) override;
	
	// Getters/setters for private variables운동량
	Vector2 GetDForcee() const { return mDForce; }
	int GetFireKey() const { return mForceKey; }
	
	void SetDForce(Vector2 force) { mDForce = force; }
	void SetForceKey(int key) { mForceKey = key; }

private:
	// 추진력(Driving Forc)
	Vector2 mDForce;
	// 추진력을 입력하는 키
	int mForceKey;


};