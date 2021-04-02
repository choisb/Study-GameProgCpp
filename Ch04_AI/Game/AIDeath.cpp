#include "AIDeath.h"
#include "AIComponent.h"
AIDeath::AIDeath(AIComponent* owner)
    :AIState(owner)
{
}

void AIDeath::Update(float deltaTime)
{
    // Death 상태에서는 더 이상의 상태 갱신은 없음.
}
