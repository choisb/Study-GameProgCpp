#include <SDL/SDL.h>
#include "AIComponent.h"
#include "Actor.h"
#include "AIState.h"

AIComponent::AIComponent(Actor* owner)
    :Component(owner)
{

}

void AIComponent::RegisterState(AIState* state)
{
    mStateMap.emplace(state->GetName(), state);
}

void AIComponent::Update(float deltaTime)
{
    if (mCurrentState)
    {
        mCurrentState->Update(deltaTime);
    }
}

void AIComponent::ChangeState(const std::string& name)
{
    // 먼저 현재 상태를 빠져 나온다
    if (mCurrentState)
    {
        mCurrentState->OnExit();
    }

    // 맵에서 새로운 상태를 찾는다.
    auto iter = mStateMap.find(name);
    if (iter != mStateMap.end())
    {
        mCurrentState = iter->second;
        // 새로운 상태로 진입한다.
        mCurrentState->OnEnter();
    }
    else
    {
        SDL_Log("Could not find AIState %s in state map", name.c_str());
        mCurrentState = nullptr;
    }
}