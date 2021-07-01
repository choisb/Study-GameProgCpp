#include "MeshComponent.h"
#include "Shader.h"
#include "Mesh.h"
#include "Actor.h"
#include "Game.h"
#include "Renderer.h"
#include "Texture.h"
#include "VertexArray.h"

MeshComponent::MeshComponent(Actor* owner, bool isSkeletal)
    :Component(owner)
    ,mMesh(nullptr)
    ,mTextureIndex(0)
    ,mVisible(true)
    ,mIsSkeletal(isSkeletal)
{
    mOwner->GetGame()->GetRenderer()->AddMeshComp(this);
}
MeshComponent::~MeshComponent()
{
    mOwner->GetGame()->GetRenderer()->RemoveMeshComp(this);
}

void MeshComponent::Draw(Shader* shader)
{
    if (mMesh)
    {
        // 세계 변환 설정
        shader->SetMatrixUniform("uWorldTransform", mOwner->GetWorldTransform());
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