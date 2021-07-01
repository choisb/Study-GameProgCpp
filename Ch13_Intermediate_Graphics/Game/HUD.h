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

    // HUD�� �ؽ�ó��

    class Texture* mRadar;
    class Texture* mHealthBar;
    class Texture* mBlipTex;
    class Texture* mRadarArrow;

    // ���ӳ� ��� Ÿ�� ������Ʈ
    std::vector<class TargetComponent*> mTargetComps;
    // ���̴��� ������� ��ȣ�� 2D ������
    std::vector<Vector2> mBlips;
    // ���̴��� ����, ��������
    float mRadarRange;
    float mRadarRadius;
    // ���ڼ��� ���� �����ϰ� �ִ���
    bool mTargetEnemy;
};