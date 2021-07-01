#include "SkeletalMeshComponent.h"
#include "Skeleton.h"
#include "Shader.h"
#include "Animation.h"
#include "Actor.h"
#include "Mesh.h"
#include "Texture.h"
#include "VertexArray.h"
#include "MatrixPalette.h"
#include "SDL/SDL.h"
#include <vector>

SkeletalMeshComponent::SkeletalMeshComponent(Actor* owner)
    :MeshComponent(owner, true)
    ,mSkeleton(nullptr)
{
}
void SkeletalMeshComponent::Draw(Shader* shader)
{
    if (mMesh)
    {
        // 세계 변환 설정
        shader->SetMatrixUniform("uWorldTransform", mOwner->GetWorldTransform());
        // matrix palette 설정
        shader->SetMatrixUniform("uMatrixPalette", &mPalette.mEntry[0], MAX_SKELETON_BONES);
        // 정반사 지수 설정
        shader->SetFloatUniform("uSpecPower", mMesh->GetSpecPower());

        // 활성화된 텍스처 설정
        Texture* t = mMesh->GetTexture(mTextureIndex);
        if (t)
        {
            t->SetActive();
        }
        // 활성화될 메시의 버텍스 배열 설정
        VertexArray* va = mMesh->GetVertexArray();
        va->SetActive();
        // 활성화된 버텍스 메시를 그린다.
        glDrawElements(GL_TRIANGLES, va->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
    }
}

void SkeletalMeshComponent::ComputeMatrixPalette()
{
    const std::vector<Matrix4>& globalInvBindPoses = mSkeleton->GetGlobalInvBindPoses();
    std::vector<Matrix4> currentPoses;
    mAnimation->GetGlobalPoseAtTime(currentPoses, mSkeleton, mAnimTime);

    // 각 본의 팔레트를 설정
    for (size_t i = 0; i < mSkeleton->GetNumBones(); i++)
    {
        // (전역 인버스 바인드 포즈 행렬) * (현재 포즈 행렬)
        mPalette.mEntry[i] = globalInvBindPoses[i] * currentPoses[i];
    }
}

float SkeletalMeshComponent::PlayAnimation(const Animation* anim, float playRate /* = 1.0f*/)
{
    mAnimation = anim;

    mAnimTime = 0.0f;
    mAnimPlayRate = playRate;

    if (!mAnimation) 
        return 0.0f;
    
    ComputeMatrixPalette();

    return mAnimation->GetDuration();
}
void SkeletalMeshComponent::Update(float deltaTime)
{
    if (mAnimation && mSkeleton)
    {
        mAnimTime += deltaTime * mAnimPlayRate;
        //  애니메이션 재생 시간을 초과하면 처음부터 다시 재생하도록 래핑
        while (mAnimTime > mAnimation->GetDuration())
        {
            mAnimTime -= mAnimation->GetDuration();
        }

        // 행렬 팔레트 재계산
        ComputeMatrixPalette();
    }
}