#pragma once
#include "AIState.h"
class AIAttack : public AIState
{
    AIAttack(class AIComponent* owner);

    // �� ���¿� ���� ������ ������
    void Update(float deltaTime) override;
    void OnEnter() override;
    void OnExit() override;

    const char* GetName() const override
    {
        return "Attack";
    }
};

