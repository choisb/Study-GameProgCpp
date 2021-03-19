#include "Actor.h"
#include "Component.h"
#include "Game.h"


Actor::Actor(Game* game)
    :mState(EActive)
    ,mPosition(Vector2::Zero)
    ,mScale(1.0f)
    ,mRotation(0.0f)
    ,mGame(game)
{
    mGame->AddActor(this);
}
Actor::~Actor()
{
    mGame->RemoveActor(this);
	while (!mComponents.empty())
	{
		delete mComponents.back();
	}
}
// Game이 호출하는 코드
void Actor::Update(float deltaTime)
{
	cout << "state: " << mState << endl;
	cout << "rotation: " << mRotation << endl;
    UpdateComponents(deltaTime);
    UpdateActor(deltaTime);
}
// Components들 업데이트 호출
void Actor::UpdateComponents(float deltaTime)
{
    for (auto comp : mComponents)
    {
        comp->Update(deltaTime);
    }
}
void Actor::UpdateActor(float deltaTime)
{
    // actor의 업데이트 코드
}
void Actor::AddComponent(Component* component)
{
    auto iter = mComponents.begin();
    for (; iter != mComponents.end(); iter++)
    {
        if ((*iter)->GetUpdateOrder() > component->GetUpdateOrder())
            break;
    }
    mComponents.insert(iter, component);
}
void Actor::RemoveComponent(Component* component)
{
    auto iter = std::find(mComponents.begin(), mComponents.end(), component);
    if (iter != mComponents.end())
    {
        mComponents.erase(iter);
    }
}
void Actor::ProcessInput(const uint8_t* keyState)
{
	if (mState == EActive)
	{
		for (auto comp : mComponents)
		{
			comp->ProcessInput(keyState);
		}
		ActorInput(keyState);
	}
}