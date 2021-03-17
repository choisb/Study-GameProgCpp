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
}
// Game�� ȣ���ϴ� �ڵ�
void Actor::Update(float deltaTime)
{
    UpdateComponents(deltaTime);
    UpdateActor(deltaTime);
}
// Components�� ������Ʈ ȣ��
void Actor::UpdateComponents(float deltaTime)
{
    for (auto comp : mComponents)
    {
        comp->Update(deltaTime);
    }
}
void Actor::UpdateActor(float deltaTime)
{
    // actor�� ������Ʈ �ڵ�
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