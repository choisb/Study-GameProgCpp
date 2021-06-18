#include "BallMove.h"
#include "Actor.h"
#include "Game.h"
#include "PhysWorld.h"
#include "BallActor.h"
#include "TargetActor.h"

BallMove::BallMove(Actor* owner)
    :MoveComponent(owner)
{

}

void BallMove::Update(float deltaTime)
{
    // �̵� �������� ������ �����Ѵ�
    const float segmentLength = 30.0f;
    Vector3 start = mOwner->GetPosition();
    Vector3 dir = mOwner->GetForward();
    Vector3 end = start + dir * segmentLength;
    
    //
    LineSegment ls(start, end);

    // ���а� ���� ������� �׽�Ʈ ����
    PhysWorld* phys = mOwner->GetGame()->GetPhysWorld();
    PhysWorld::CollisionInfo info;
    //
    if (phys->SegmentCast(ls, info) && info.mActor != mPlayer)
    {
        // �浹�ߴٸ� ǥ�� ������ �������� �ؼ� �ݻ��Ų��.
        dir = Vector3::Reflect(dir, info.mNormal);
        mOwner->RotateToNewForward(dir);

        TargetActor* target = dynamic_cast<TargetActor*>(info.mActor);
        if (target)
        {
            static_cast<BallActor*>(mOwner)->HitTarget();
        }
    }

    // ���� �ӵ��� ��Ÿ �ð����� ���̽� Ŭ���� �̵��� ����
    MoveComponent::Update(deltaTime);
}