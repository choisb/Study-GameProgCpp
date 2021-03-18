#include "InputComponent.h"

InputComponent::InputComponent(class Actor* owner)
	:MoveComponent(owner)
{
}

void InputComponent::ProcessInput(const uint8_t* keyState)
{
	// MoveComponent를 위한 전방 속도 계산
	float forwardSpeed = 0.0f;
	if (keyState[mForwardKey])
	{
		forwardSpeed += mMaxForwardSpeed;
	}
	if (keyState[mBackKey])
	{
		forwardSpeed -= mMaxForwardSpeed;
	}
	SetForwardSpeed(forwardSpeed);

	// MoveComponent를 위한 각 속도 계산
	float angularSpeed = 0.0f;
	if (keyState[mClockwiseKey])
	{
		angularSpeed += mMaxAngularSpeed;
	}
	if (keyState[mCounterClockwiseKey])
	{
		angularSpeed -= mMaxAngularSpeed;
	}
	SetAngularSpeed(angularSpeed);

}