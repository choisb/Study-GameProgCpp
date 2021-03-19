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

	// Actor�� ���°� paused��� Actor �Է� �ȹ���

	if (mOwner->GetState() == Actor::EPaused)
		return;

	// �߻�� �ѹ��� �̷��������


	if (keyState[mFireKey])
	{
		cout << "Fire!" << endl;
		AddForce(mFireForce);
		isFired = true;
	}

}