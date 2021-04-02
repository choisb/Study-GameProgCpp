#include "InputComponent.h"
#include "Actor.h"

InputComponent::InputComponent(class Actor* owner)
	:MoveComponent(owner)
	,mDForce(Vector2(0.0f,0.0f))
{
}

void InputComponent::ProcessInput(const uint8_t* keyState)
{
	// Actor의 상태가 paused라면 Actor 입력 안받음
	if (mOwner->GetState() == Actor::EPaused)
		return;

    // ForceKey가 입력되면 mDForce만큼의 힘이 더해짐.
    if (keyState[mForceKey])
    {
        AddForce(mDForce);
    }
}