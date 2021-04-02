#include "AIPatrol.h"
#include "AIComponent.h"
AIPatrol::AIPatrol(AIComponent* owner)
    :AIState(owner)
{
}

void AIPatrol::Update(float deltaTime)
{
    // �Ϻ� ���� �۾�
    // ...

    bool dead = false;  // �ڽ��� �׾����� �Ǵ��ϴ� �Լ� �ۼ� �ʿ�
    if (dead)
    {
        // AI ������Ʈ�� ���¸� �����϶�� �˸�
        mOwner->ChangeState("Death");
    }
}
