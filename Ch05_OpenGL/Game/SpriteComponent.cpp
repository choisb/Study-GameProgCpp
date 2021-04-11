#include "SpriteComponent.h"
#include "Actor.h"
#include "Game.h"
#include "Shader.h"

SpriteComponent::SpriteComponent(Actor* owner, int drawOrder)
    : Component(owner)
    , mTexture(nullptr)
    , mDrawOrder(drawOrder)
    , mTexWidth(0)
    , mTexHeight(0)
{
    mOwner->GetGame()->AddSprite(this);
}
SpriteComponent::~SpriteComponent()
{
    mOwner->GetGame()->RemoveSprite(this);
}

void SpriteComponent::SetTexture(SDL_Texture* texture)
{
    mTexture = texture;
    // �ؽ�ó�� �ʺ�� ���̸� �˱� ���ؼ� SDL_QuertTexture() �Լ� ���
    SDL_QueryTexture(texture, nullptr, nullptr, &mTexWidth, &mTexHeight);
}
void SpriteComponent::Draw(Shader* shader)
{
    // glDrawElements ȣ���� ���ؼ��� Ȱ��ȭ�� ���ؽ� �迭 ��ü�� Ȱ��ȭ�� ���̴��� �ʿ��ϴ�.
    // �� �����ӿ��� SpriteComponents�� �׸��� ���� ��������Ʈ ���ؽ� �迭��ü�� ���̴� ��θ� Ȱ��ȭ �ؾ��Ѵ�.
    glDrawElements(
        GL_TRIANGLES,   // �׷��� �� ������ Ÿ��
        6,              // �ε��� ���ۿ� �ִ� �ε����� ��
        GL_UNSIGNED_INT,// �� �ε����� Ÿ��
        nullptr         // nullptr
    );
}