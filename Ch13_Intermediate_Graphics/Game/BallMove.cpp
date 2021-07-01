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
    // 이동 방향으로 선분을 생성한다
    const float segmentLength = 30.0f;
    Vector3 start = mOwner->GetPosition();
    Vector3 dir = mOwner->GetForward();
    Vector3 end = start + dir * segmentLength;
    
    //
    LineSegment ls(start, end);

    // 선분과 게임 세계와의 테스트 진행
    PhysWorld* phys = mOwner->GetGame()->GetPhysWorld();
    PhysWorld::CollisionInfo info;
    //
    if (phys->SegmentCast(ls, info) && info.mActor != mPlayer)
    {
        // 충돌했다면 표면 법선을 기준으로 해서 반사시킨다.
        dir = Vector3::Reflect(dir, info.mNormal);
        mOwner->RotateToNewForward(dir);

        TargetActor* target = dynamic_cast<TargetActor*>(info.mActor);
        if (target)
        {
            static_cast<BallActor*>(mOwner)->HitTarget();
        }
    }

    // 전진 속도와 델타 시간으로 베이스 클래스 이동을 갱신
    MoveComponent::Update(deltaTime);
}