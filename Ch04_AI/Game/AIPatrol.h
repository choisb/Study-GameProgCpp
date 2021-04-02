#pragma once
#include "AIState.h"
class AIPatrol : public AIState
{
public:
    AIPatrol(class AIComponent* owner);

    // �� ���¿� ���� ������ ������
    void Update(float deltaTime) override;
    void OnEnter() override;
    void OnExit() override;

    const char* GetName() const override
    {
        return "Patrol";
    }
};