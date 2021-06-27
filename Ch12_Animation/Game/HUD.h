#pragma once
#include "UIScreen.h"
#include <vector>

class HUD : public UIScreen
{
public:
    HUD(class Game* game);
    ~HUD();

    void Update(float deltaTime) override;
    void Draw(class Shader* shader) override;

    void AddTargetComponent(class TargetComponent* tc);
    void RemoveTargetComponent(class TargetComponent* tc);
    //void Draw(class Shader* shader) override;

protected:
    void UpdateCrosshair(float deltaTime);
    void UpdateRadar(float deltaTime);

    // HUD의 텍스처들

    class Texture* mRadar;
    class Texture* mHealthBar;
    class Texture* mBlipTex;
    class Texture* mRadarArrow;

    // 게임내 모든 타겟 컴포넌트
    std::vector<class TargetComponent*> mTargetComps;
    // 레이더에 상대적인 신호의 2D 오프셋
    std::vector<Vector2> mBlips;
    // 레이더의 범위, 반지름값
    float mRadarRange;
    float mRadarRadius;
    // 십자선이 적을 조준하고 있는지
    bool mTargetEnemy;
};