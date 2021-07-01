#pragma once
#include <vector>
#include <functional>
#include "Math.h"
#include "Collision.h"

class PhysWorld
{
public:
    PhysWorld(class Game* game);

    // Used to give helpful information about collision results
    struct CollisionInfo
    {
        // 충돌 지점
        Vector3 mPoint;
        // 충돌 시의 법선 벡터
        Vector3 mNormal;
        // 충돌한 컴포넌트
        class BoxComponent* mBox;
        // 컴포넌트의 소유자 엑터
        class Actor* mActor;
    };

    // 박스 컴포넌트를 추가하거나/제거
    void AddBox(class BoxComponent* box);
    void RemoveBox(class BoxComponent* box);

    // 모든 박스에대해서 광선충돌 테스트 진행
    // 충돌이 발생하면 true를 반환하고 outColl에 충돌 정보를 저장
    bool SegmentCast(const LineSegment& l, CollisionInfo& outColl);

    // 모든 박스에 대한 충돌 검사 (효율성이 다소 떨어짐)
    void TestPairwise(std::function<void(class Actor*, class Actor*)> f);
    // 모든 박스에 대한 충돌 검사 (최적화된 방법)
    void TestSweepAndPrune(std::function<void(class Actor*, class Actor*)> f);

private:
    class Game* mGame;
    std::vector<class BoxComponent*> mBoxes;
};