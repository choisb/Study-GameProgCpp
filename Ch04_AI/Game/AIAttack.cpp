#include "AIAttack.h"
#include "AIComponent.h"
AIAttack::AIAttack(AIComponent* owner)
    :AIState(owner)
{
}

void AIAttack::Update(float deltaTime)
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
