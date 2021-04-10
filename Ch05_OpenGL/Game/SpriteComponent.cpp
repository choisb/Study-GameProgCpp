#include "SpriteComponent.h"
#include "Actor.h"
#include "Game.h"

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
void SpriteComponent::Draw(SDL_Renderer* renderer)
{
    if (mTexture)
    {
        SDL_Rect r;
        // mOwner의 스케일 값으로 너비/높이 설정
        r.w = static_cast<int>(mTexWidth * mOwner->GetScale());
        r.h = static_cast<int>(mTexHeight * mOwner->GetScale());
        // 소유자의 위치를 중심으로 사각형 배치
        r.x = static_cast<int>(mOwner->GetPosition().x - r.w / 2);
        r.y = static_cast<int>(mOwner->GetPosition().y - r.h / 2);

        // 스프라이트를 그린다.
        SDL_RenderCopyEx(renderer,
            mTexture,       // 그릴 텍스처
            nullptr,        // 그릴 텍스처의 영역, nullptr이면 전체 영역
            &r,             // 그려질 대상의 사각형 영역
            -Math::ToDegrees(mOwner->GetRotation()),    // 라디안을 각도로 변환
            nullptr,        // 회전 기준점
            SDL_FLIP_NONE); // 플립 행동
    }
}