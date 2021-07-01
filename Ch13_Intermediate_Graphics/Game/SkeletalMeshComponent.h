#pragma once
#include "MeshComponent.h"
#include "MatrixPalette.h"
class SkeletalMeshComponent : public MeshComponent
{
public:
    SkeletalMeshComponent(class Actor* owner);
    // �޽�������Ʈ�� �׸���.
    void Draw(class Shader* shader) override;

    void Update(float deltaTime) override;

    // Setter
    void SetSkeleton(const class Skeleton* sk) { mSkeleton = sk; }

    // �ִϸ��̼��� ���. �ִϸ��̼��� ���̸� ��ȯ�Ѵ�.
    float PlayAnimation(const class Animation* anim, float playRate = 1.0f);
protected:
    void ComputeMatrixPalette();

    // ��� �ȷ�Ʈ
    MatrixPalette mPalette;
    // ���� ����ǰ� �ִ� �ִϸ��̼�
    const class Animation* mAnimation;
    const class Skeleton* mSkeleton;
    // �ִϸ��̼� ��� �ӵ� (1.0�� ���� �ӵ�)
    float mAnimPlayRate;
    // �ִϸ��̼� ���� �ð�
    float mAnimTime;
};

