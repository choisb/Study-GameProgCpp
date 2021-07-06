#include "HUD.h"
#include "Texture.h"
#include "Shader.h"
#include "Game.h"
#include "Renderer.h"
#include "PhysWorld.h"
#include "FollowActor.h"
#include "TargetComponent.h"
#include "MirrorCamera.h"
#include <algorithm>

HUD::HUD(Game* game)
    :UIScreen(game)
    ,mTargetEnemy(false)
    ,mRadarRadius(92.0f)
    ,mRadarRange(2000.0f)
{
    Renderer* r = mGame->GetRenderer();
    mRadar = r->GetTexture("../Assets/Radar.png");
    mBlipTex = r->GetTexture("../Assets/Blip.png");
    mRadarArrow = r->GetTexture("../Assets/RadarArrow.png");
    mHealthBar = r->GetTexture("../Assets/HealthBar.png");
}
HUD::~HUD()
{

}

void HUD::Update(float deltaTime)
{
    UIScreen::Update(deltaTime);

    UpdateCrosshair(deltaTime);
    UpdateRadar(deltaTime);

}
void HUD::Draw(Shader* shader)
{

    // Radar
    const Vector2 cRadarPos(-390.0f, 275.0f);
    DrawTexture(shader, mRadar, cRadarPos, 1.0f);

    // Blips
    for (Vector2& blip : mBlips)
    {
        DrawTexture(shader, mBlipTex, cRadarPos + blip, 1.0f);
    }

    // Radar arrow
    DrawTexture(shader, mRadarArrow, cRadarPos);

    Texture* mirror = mGame->GetRenderer()->GetMirrorTexture();
    DrawTexture(shader, mirror, Vector2(-350.0f, -250.0f), 1.0f, true);

}
void HUD::AddTargetComponent(TargetComponent* tc)
{
    mTargetComps.emplace_back(tc);
}
void HUD::RemoveTargetComponent(TargetComponent* tc)
{
    auto iter = std::find(mTargetComps.begin(), mTargetComps.end(), tc);
    mTargetComps.erase(iter);
}
void HUD::UpdateCrosshair(float deltaTime)
{
    // �Ϲ� Ŀ���� ����
    mTargetEnemy = false;
    // ������ �����.
    // ������ ����(��ȿ ��Ÿ�)
    const float cAimDist = 5000.0f;
    Vector3 start, dir;
    mGame->GetRenderer()->GetScreenDirection(start, dir);
    LineSegment l(start, start + dir * cAimDist);
    // ���� ĳ��Ʈ
    PhysWorld::CollisionInfo info;
    if (mGame->GetPhysWorld()->SegmentCast(l, info))
    {
        // SegmentCast�� ���� ���� ���Ͱ� Ÿ�� ������Ʈ�� �����ڿ� ��ġ�ϴ����� üũ
        for (auto tc : mTargetComps)
        {
            if (tc->GetOwner() == info.mActor)
            {
                mTargetEnemy = true;
                break;
            }
        }
    }
}
void HUD::UpdateRadar(float deltaTime)
{
    // ��ȣ ������ ���� Ŭ����
    mBlips.clear();

    // �÷��̾� ��ġ�� ���̴� ��ǥ�� ��ȯ
    // 3D������ x���� ����
    // 2D������ y���� ����
    // ������ (x, y, z) ��ǥ�� (y, x) ��ǥ�� ����
    Vector3 playerPos = mGame->GetPlayer()->GetPosition();
    Vector2 playerPos2D(playerPos.y, playerPos.x);

    // ���̴��� ������ �÷��̾��� ���������
    Vector3 playerForward = mGame->GetPlayer()->GetForward();
    Vector2 playerForward2D(playerForward.x, playerForward.y);

    // ���̴��� ȸ����Ű�� ���� atan2 �Լ� ���
    float angle = Math::Atan2(playerForward2D.y, playerForward2D.x);
    // 2D ȸ�� ����� �����.
    Matrix3 rotMat = Matrix3::CreateRotation(angle);

    // ������Ʈ ��ȣ�� ��ġ�� ��´�
    for (auto tc : mTargetComps)
    {
        Vector3 targetPos = tc->GetOwner()->GetPosition();
        Vector2 actorPos2D(targetPos.y, targetPos.x);

        // �÷��̾�� Ÿ�� ������ ���͸� ���
        Vector2 playerToTarget = actorPos2D - playerPos2D;

        // Ÿ���� ���� �ȿ� �ִٸ�
        if (playerToTarget.LengthSq() <= (mRadarRange * mRadarRange))
        {
            // playerToTarget ��ǥ��
            // ���̴� ȭ�� �߽����κ��� ���������� ��ȯ
            Vector2 blipPos = playerToTarget;
            blipPos *= mRadarRadius / mRadarRange;
            // ��ȣ�� ȸ������ ���̴� ������ ���� ��ǥ�� ��ȯ
            blipPos = Vector2::Transform(blipPos, rotMat);
            mBlips.emplace_back(blipPos);
        }
    }
}