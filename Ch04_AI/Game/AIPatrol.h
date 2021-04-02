#pragma once
#include "AIState.h"
class AIPatrol : public AIState
{
public:
    AIPatrol(class AIComponent* owner);

    // 이 상태에 대한 행위를 재정의
    void Update(float deltaTime) override;
    void OnEnter() override;
    void OnExit() override;

    const char* GetName() const override
    {
        return "Patrol";
    }
};