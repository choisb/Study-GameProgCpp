#include "InputComponent.h"
#include "Actor.h"

InputComponent::InputComponent(class Actor* owner)
	:MoveComponent(owner)
	,mFireForce(Vector2(0.0f,0.0f))
	,isFired(false)
{
}

void InputComponent::ProcessInput(const uint8_t* keyState)
{

	// Actor의 상태가 paused라면 Actor 입력 안받음

	if (mOwner->GetState() == Actor::EPaused)
		return;

	// 발사는 한번만 이루어져야함


	if (keyState[mFireKey])
	{
		cout << "Fire!" << endl;
		AddForce(mFireForce);
		isFired = true;
	}

}