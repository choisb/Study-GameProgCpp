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
    // 텍스처의 너비와 높이로 사각형을 스케일
    Matrix4 scaleMat = Matrix4::CreateScale(
        static_cast<float>(mTexWidth),
        static_cast<float>(mTexHeight),
        1.0f);

    Matrix4 world = scaleMat * mOwner->GetWorldTransform();
    // 세계 변환 행렬을 설정
    shader->SetMatrixUniform("uWorldTransform", world);
    // 현재 텍스처를 설정
    mTexture->SetActive();
    // 사각형을 그린다.
    // glDrawElements 호출을 위해서는 활성화된 버텍스 배열 개체와 활성화된 셰이더가 필요하다.
    // 매 프레임에서 SpriteComponents를 그리기 전에 스프라이트 버텍스 배열개체와 셰이더 모두를 활성화 해야한다.
    glDrawElements(
        GL_TRIANGLES,   // 그려야 할 폴리곤 타입
        6,              // 인덱스 버퍼에 있는 인덱스의 수
        GL_UNSIGNED_INT,// 각 인덱스의 타입
        nullptr         // nullptr
    );
}