#include "AnimSpriteComponent.h"
#include "Actor.h"

AnimSpriteComponent::AnimSpriteComponent(Actor* owner, int drawOrder = 100)
    :SpriteComponent(owner, drawOrder)
    , mCurrFrame(0.0f)
    , mAnimFPS(0.0f)
{

}

void AnimSpriteComponent::Update(float deltaTime)
{
    SpriteComponent::Update(deltaTime);

    if (mAnimTextures.size() > 0)
    {
        // 프레임 레이트와 델타 시간을 기반으로
        // 현재 프레임 갱신
        mCurrFrame += mAnimFPS * deltaTime;

        // 애니메이션 텍스처 수를 초과하면 현재 프레임을 래핑한다.
        while (mCurrFrame >= mAnimTextures.size())
        {
            mCurrFrame -= mAnimTextures.size();
        }

        // 현재 텍스처를 설정
        SetTexture(mAnimTextures[static_cast<int> (mCurrFrame)]);
    }
}

void AnimSpriteComponent::SetAnimTextures(const std::vector<SDL_Texture*>& textures)
{
    mAnimTextures = textures;
    if (mAnimTextures.size() > 0)
    {
        // Set the active texture to first frame
        mCurrFrame = 0.0f;
        SetTexture(mAnimTextures[0]);
    }
}