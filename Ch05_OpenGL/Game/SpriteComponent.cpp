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
    // 텍스처의 너비와 높이를 알기 위해서 SDL_QuertTexture() 함수 사용
    SDL_QueryTexture(texture, nullptr, nullptr, &mTexWidth, &mTexHeight);
}
void SpriteComponent::Draw(Shader* shader)
{
    // glDrawElements 호출을 위해서는 활성화된 버텍스 배열 개체와 활성화된 셰이더가 필요하다.
    // 매 프레임에서 SpriteComponents를 그리기 전에 스프라이트 버텍스 배열개체와 셰이더 모두를 활성화 해야한다.
    glDrawElements(
        GL_TRIANGLES,   // 그려야 할 폴리곤 타입
        6,              // 인덱스 버퍼에 있는 인덱스의 수
        GL_UNSIGNED_INT,// 각 인덱스의 타입
        nullptr         // nullptr
    );
}