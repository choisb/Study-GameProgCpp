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
        // ���� ��ȯ ����
        shader->SetMatrixUniform("uWorldTransform", mOwner->GetWorldTransform());
        // matrix palette ����
        shader->SetMatrixUniform("uMatrixPalette", &mPalette.mEntry[0], MAX_SKELETON_BONES);
        // ���ݻ� ���� ����
        shader->SetFloatUniform("uSpecPower", mMesh->GetSpecPower());

        // Ȱ��ȭ�� �ؽ�ó ����
        Texture* t = mMesh->GetTexture(mTextureIndex);
        if (t)
        {
            t->SetActive();
        }
        // Ȱ��ȭ�� �޽��� ���ؽ� �迭 ����
        VertexArray* va = mMesh->GetVertexArray();
        va->SetActive();
        // Ȱ��ȭ�� ���ؽ� �޽ø� �׸���.
        glDrawElements(GL_TRIANGLES, va->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
    }
}

void SkeletalMeshComponent::ComputeMatrixPalette()
{
    const std::vector<Matrix4>& globalInvBindPoses = mSkeleton->GetGlobalInvBindPoses();
    std::vector<Matrix4> currentPoses;
    mAnimation->GetGlobalPoseAtTime(currentPoses, mSkeleton, mAnimTime);

    // �� ���� �ȷ�Ʈ�� ����
    for (size_t i = 0; i < mSkeleton->GetNumBones(); i++)
    {
        // (���� �ι��� ���ε� ���� ���) * (���� ���� ���)
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
        //  �ִϸ��̼� ��� �ð��� �ʰ��ϸ� ó������ �ٽ� ����ϵ��� ����
        while (mAnimTime > mAnimation->GetDuration())
        {
            mAnimTime -= mAnimation->GetDuration();
        }

        // ��� �ȷ�Ʈ ����
        ComputeMatrixPalette();
    }
}