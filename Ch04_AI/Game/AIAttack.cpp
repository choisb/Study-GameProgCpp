#include "AIAttack.h"
#include "AIComponent.h"
AIAttack::AIAttack(AIComponent* owner)
    :AIState(owner)
{
}

void AIAttack::Update(float deltaTime)
{
    // 일부 갱신 작업
    // ...

    bool dead = false;  // 자신이 죽었는지 판단하는 함수 작성 필요
    if (dead)
    {
        // AI 컴포넌트에 상태를 변경하라고 알림
        mOwner->ChangeState("Death");
    }
}
