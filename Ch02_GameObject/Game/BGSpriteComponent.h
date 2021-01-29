#pragma once
#include "SpriteComponent.h"
#include "Math.h"
#include <vector>

class BGSpriteComponent : public SpriteComponent 
{
public:
    // 기본 그리기 순서값을 낮은 값으로 설정 (배경이므로)
    BGSpriteComponent(class Actor* owner, int drawOrder = 10);

    // 부모로부터 오버라이딩한 Update/draw 함수
    void Update(float deltaTime) override;
    void Draw(SDL_Renderer* renderer) override;

    // 배경에 사용되는 텍스처 설정
    void SetBGTextures(const std::vector<SDL_Texture*>& textures);

    //화면 크기를 얻거나 스크롤 속도 값을 설정 및 얻는 함수
    void SetScreenSize(const Vector2& size) { mScreenSize = size; }
    void SetScrollSpeed(float speed) { mScrollSpeed = speed; }
    float GetScrollSpeed() const { return mScrollSpeed; }


private:
    // 각 배경 이미지와 배경 이미지의 오프셋 값을 캡슐화한 구조체
    struct BGTexture
    {
        SDL_Texture* mTexture;
        Vector2 mOffset;
    };

    std::vector<BGTexture> mBGTextures;
    Vector2 mScreenSize;
    float mScrollSpeed;
};