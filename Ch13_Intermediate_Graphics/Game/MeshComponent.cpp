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
        // ���� ��ȯ ����
        shader->SetMatrixUniform("uWorldTransform", mOwner->GetWorldTransform());
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