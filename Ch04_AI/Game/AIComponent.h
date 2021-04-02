#pragma once
#include "Component.h"
#include <string>
#include <unordered_map>

class AIComponent : public Component
{
public:
    AIComponent(class Actor* owner);

    void Update(float deltaTime) override;
    void ChangeState(const std::string& name);

    // �� ���¸� �ʿ� �߰��Ѵ�.
    void RegisterState(class AIState* state);

private:
    // ������ �̸��� AIState �ν��Ͻ��� �����Ѵ�.
    std::unordered_map<std::string, class AIState*> mStateMap;
    // AI�� ���� ����
    class AIState* mCurrentState;
};