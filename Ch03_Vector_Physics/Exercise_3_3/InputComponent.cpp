#include "InputComponent.h"
#include "Actor.h"

InputComponent::InputComponent(class Actor* owner)
	:MoveComponent(owner)
	,mDForce(Vector2(0.0f,0.0f))
{
}

void InputComponent::ProcessInput(const uint8_t* keyState)
{
	// Actor�� ���°� paused��� Actor �Է� �ȹ���
	if (mOwner->GetState() == Actor::EPaused)
		return;

    // ForceKey�� �ԷµǸ� mDForce��ŭ�� ���� ������.
    if (keyState[mForceKey])
    {
        AddForce(mDForce);
    }
}