#pragma once
#include "MeshComponent.h"
#include "MatrixPalette.h"
class SkeletalMeshComponent : public MeshComponent
{
public:
    SkeletalMeshComponent(class Actor* owner);
    // 메시컴포넌트를 그린다.
    void Draw(class Shader* shader) override;

    void Update(float deltaTime) override;

    // Setter
    void SetSkeleton(const class Skeleton* sk) { mSkeleton = sk; }

    // 애니메이션을 재생. 애니메이션의 길이를 반환한다.
    float PlayAnimation(const class Animation* anim, float playRate = 1.0f);
protected:
    void ComputeMatrixPalette();

    // 행렬 팔레트
    MatrixPalette mPalette;
    // 현재 재생되고 있는 애니메이션
    const class Animation* mAnimation;
    const class Skeleton* mSkeleton;
    // 애니메이션 재생 속도 (1.0이 정상 속도)
    float mAnimPlayRate;
    // 애니메이션 현재 시간
    float mAnimTime;
};

