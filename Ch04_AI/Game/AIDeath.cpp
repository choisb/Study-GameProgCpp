#include "AIDeath.h"
#include "AIComponent.h"
AIDeath::AIDeath(AIComponent* owner)
    :AIState(owner)
{
}

void AIDeath::Update(float deltaTime)
{
    // Death ���¿����� �� �̻��� ���� ������ ����.
}
