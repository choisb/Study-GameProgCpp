#pragma once

class AIState
{
public:
    AIState(class AIComponent * owner)
        :mOwner(owner)
    {}
    // 각 상태의 구체적인 행동
    virtual void Update(float deltaTime) = 0;
    virtual void OnEnter() = 0;
    virtual void OnExit() = 0;

    // 상태의 이름 얻기
    virtual const char* GetName() const = 0;
protected:
    class AIComponent* mOwner;
};