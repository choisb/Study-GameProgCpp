#pragma once
#include "MoveComponent.h"
#include <cstdint>

class InputComponent : public MoveComponent
{
public:
	InputComponent(class Actor* owner);
	void ProcessInput(const struct InputState& state) override;
	
	// Getters/setters for private variables
	float GetMaxForward() const { return mMaxForwardSpeed; }
	float GetMaxAngular() const { return mMaxAngularSpeed; }
	int GetForwardKey() const { return mForwardKey; }
	int GetBackKey() const { return mBackKey; }
	int GetClockwiseKey() const { return mClockwiseKey; }
	int GetCounterClockwiseKey() const { return mCounterClockwiseKey; }

	void SetMaxForwardSpeed(float speed) { mMaxForwardSpeed = speed; }
	void SetMaxAngularSpeed(float speed) { mMaxAngularSpeed = speed; }
	void SetForwardKey(int key) { mForwardKey = key; }
	void SetBackKey(int key) { mBackKey = key; }
	void SetClockwiseKey(int key) { mClockwiseKey = key; }
	void SetCounterClockwiseKey(int key) { mCounterClockwiseKey = key; }

private:
	// 최대 전방 속도 / 최대 각 속도
	float mMaxForwardSpeed;
	float mMaxAngularSpeed;
	// 전진 및 후진을 위한 키
	int mForwardKey;
	int mBackKey;
	// 각 운동을 위한 키
	int mClockwiseKey;
	int mCounterClockwiseKey;

};