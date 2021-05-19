#include "InputComponent.h"
#include "InputSystem.h"

InputComponent::InputComponent(class Actor* owner)
	:MoveComponent(owner)
{
}

void InputComponent::ProcessInput(const InputState& state)
{
	// MoveComponent�� ���� ���� �ӵ� ���
	float forwardSpeed = 0.0f;
	if (state.Keyboard.GetKeyValue(SDL_Scancode(mForwardKey)))
	{
		forwardSpeed += mMaxForwardSpeed;
	}
	if (state.Keyboard.GetKeyValue(SDL_Scancode(mBackKey)))
	{
		forwardSpeed -= mMaxForwardSpeed;
	}
	SetForwardSpeed(forwardSpeed);

	// MoveComponent�� ���� �� �ӵ� ���
	float angularSpeed = 0.0f;
	if (state.Keyboard.GetKeyValue(SDL_Scancode(mClockwiseKey)))
	{
		angularSpeed += mMaxAngularSpeed;
	}
	if (state.Keyboard.GetKeyValue(SDL_Scancode(mCounterClockwiseKey)))
	{
		angularSpeed -= mMaxAngularSpeed;
	}
	SetAngularSpeed(angularSpeed);

}