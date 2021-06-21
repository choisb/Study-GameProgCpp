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
        // �浹 ����
        Vector3 mPoint;
        // �浹 ���� ���� ����
        Vector3 mNormal;
        // �浹�� ������Ʈ
        class BoxComponent* mBox;
        // ������Ʈ�� ������ ����
        class Actor* mActor;
    };

    // �ڽ� ������Ʈ�� �߰��ϰų�/����
    void AddBox(class BoxComponent* box);
    void RemoveBox(class BoxComponent* box);

    // ��� �ڽ������ؼ� �����浹 �׽�Ʈ ����
    // �浹�� �߻��ϸ� true�� ��ȯ�ϰ� outColl�� �浹 ������ ����
    bool SegmentCast(const LineSegment& l, CollisionInfo& outColl);

    // ��� �ڽ��� ���� �浹 �˻� (ȿ������ �ټ� ������)
    void TestPairwise(std::function<void(class Actor*, class Actor*)> f);
    // ��� �ڽ��� ���� �浹 �˻� (����ȭ�� ���)
    void TestSweepAndPrune(std::function<void(class Actor*, class Actor*)> f);

private:
    class Game* mGame;
    std::vector<class BoxComponent*> mBoxes;
};