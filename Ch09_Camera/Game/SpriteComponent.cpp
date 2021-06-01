#include "SpriteComponent.h"
#include "Actor.h"
#include "Game.h"
#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"

SpriteComponent::SpriteComponent(Actor* owner, int drawOrder)
    : Component(owner)
    , mTexture(nullptr)
    , mDrawOrder(drawOrder)
    , mTexWidth(0)
    , mTexHeight(0)
    , mVisible(true)
{
    mOwner->GetGame()->GetRenderer()->AddSprite(this);
}
SpriteComponent::~SpriteComponent()
{
    mOwner->GetGame()->GetRenderer()->RemoveSprite(this);
}

void SpriteComponent::SetTexture(Texture* texture)
{
    mTexture = texture;
    mTexHeight = texture->GetHeight();
    mTexWidth = texture->GetWidth();
}
void SpriteComponent::Draw(Shader* shader)
{
    // �ؽ�ó�� �ʺ�� ���̷� �簢���� ������
    Matrix4 scaleMat = Matrix4::CreateScale(
        static_cast<float>(mTexWidth),
        static_cast<float>(mTexHeight),
        1.0f);

    Matrix4 world = scaleMat * mOwner->GetWorldTransform();
    // ���� ��ȯ ����� ����
    shader->SetMatrixUniform("uWorldTransform", world);
    // ���� �ؽ�ó�� ����
    mTexture->SetActive();
    // �簢���� �׸���.
    // glDrawElements ȣ���� ���ؼ��� Ȱ��ȭ�� ���ؽ� �迭 ��ü�� Ȱ��ȭ�� ���̴��� �ʿ��ϴ�.
    // �� �����ӿ��� SpriteComponents�� �׸��� ���� ��������Ʈ ���ؽ� �迭��ü�� ���̴� ��θ� Ȱ��ȭ �ؾ��Ѵ�.
    glDrawElements(
        GL_TRIANGLES,   // �׷��� �� ������ Ÿ��
        6,              // �ε��� ���ۿ� �ִ� �ε����� ��
        GL_UNSIGNED_INT,// �� �ε����� Ÿ��
        nullptr         // nullptr
    );
}